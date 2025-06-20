/*
 * Copyright 2021 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Author:
 *     Jan Černý <jcerny@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <libxslt/xslt.h>
#include <libxslt/security.h>

#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/crypto.h>

#include "common/_error.h"
#include "common/debug_priv.h"
#include "oscap_source.h"
#include "oscap_source_priv.h"
#include "signature_priv.h"

struct oscap_signature_ctx {
	const char *pubkey_pem; // path to the public key file in PEM format
	const char *pubkey_cert_pem; // path to the public key certificate file in PEM format
};

struct oscap_signature_ctx *oscap_signature_ctx_new()
{
	struct oscap_signature_ctx *ctx = malloc(sizeof(struct oscap_signature_ctx));
	ctx->pubkey_pem = NULL;
	ctx->pubkey_cert_pem = NULL;
	return ctx;
}

void oscap_signature_ctx_set_pubkey_pem(struct oscap_signature_ctx *ctx, const char *path)
{
	ctx->pubkey_pem = path;
}

void oscap_signature_ctx_set_pubkey_cert_pem(struct oscap_signature_ctx *ctx, const char *path)
{
	ctx->pubkey_cert_pem = path;
}

void oscap_signature_ctx_free(struct oscap_signature_ctx *ctx)
{
	free(ctx);
}

static int _mark_sds_id_elements(xmlDocPtr doc)
{
	/* Adds id attibutes of ds:component and ds:data-stream to the list
	 * of globally known id attributes.
	 */
	xmlNodePtr root = xmlDocGetRootElement(doc);
	xmlNodePtr node = root->children;
	for (; node != NULL; node = node->next) {
		if (node->type != XML_ELEMENT_NODE)
			continue;
		const char *name = (const char *)node->name;
		if (strcmp(name, "component") && strcmp(name, "data-stream"))
			continue;
		xmlAttrPtr id_attr = xmlHasProp(node, BAD_CAST "id");
		if (id_attr == NULL || id_attr->children == NULL) {
			oscap_seterr(OSCAP_EFAMILY_XML, "Element \"%s\" doesn't have an id attribute\n", name);
			return -1;
		}
		xmlChar *id_attr_value = xmlNodeListGetString(node->doc, id_attr->children, 1);
		if (id_attr_value == NULL) {
			oscap_seterr(OSCAP_EFAMILY_XML, "Attribute \"id\" in element \"%s\" has no value\n", name);
			return -1;
		}
		/* check that we don't have same ID already */
		xmlAttrPtr tmp_attr = xmlGetID(node->doc, id_attr_value);
		if (tmp_attr == NULL) {
			xmlAddID(NULL, node->doc, id_attr_value, id_attr);
		} else if (tmp_attr != id_attr) {
			oscap_seterr(OSCAP_EFAMILY_XML, "Duplicate ID attribute \"%s\"\n", id_attr_value);
			xmlFree(id_attr_value);
			return -1;
		}
		xmlFree(id_attr_value);
	}
	return 0;
}

static int _oscap_signature_validate_doc(xmlDocPtr doc, oscap_document_type_t scap_type, struct oscap_signature_ctx *ctx, bool enforce_signature)
{
	int res = -1;
	xmlNodePtr node = NULL;
	xmlSecDSigCtxPtr dsigCtx = NULL;
	xmlSecKeysMngrPtr mngr = NULL;
	xsltSecurityPrefsPtr xsltSecPrefs = NULL;

	dI("Validating XML signature.");

	/* Init libxslt */
	/* disable everything */
	xsltSecPrefs = xsltNewSecurityPrefs();
	xsltSetSecurityPrefs(xsltSecPrefs, XSLT_SECPREF_READ_FILE, xsltSecurityForbid);
	xsltSetSecurityPrefs(xsltSecPrefs, XSLT_SECPREF_WRITE_FILE, xsltSecurityForbid);
	xsltSetSecurityPrefs(xsltSecPrefs, XSLT_SECPREF_CREATE_DIRECTORY, xsltSecurityForbid);
	xsltSetSecurityPrefs(xsltSecPrefs, XSLT_SECPREF_READ_NETWORK, xsltSecurityForbid);
	xsltSetSecurityPrefs(xsltSecPrefs, XSLT_SECPREF_WRITE_NETWORK, xsltSecurityForbid);
	xsltSetDefaultSecurityPrefs(xsltSecPrefs);

	/* Init xmlsec library */
	if (xmlSecInit() < 0) {
		oscap_seterr(OSCAP_EFAMILY_XML, "Xmlsec initialization failed.");
		return(-1);
	}

	/* Check loaded library version */
	if (xmlSecCheckVersion() != 1) {
		oscap_seterr(OSCAP_EFAMILY_XML, "Loaded xmlsec library version is not compatible.");
		return(-1);
	}

	/* Init crypto library */
	if (xmlSecCryptoAppInit(NULL) < 0) {
		oscap_seterr(OSCAP_EFAMILY_XML, "Crypto initialization failed.");
		return(-1);
	}

	/* Init xmlsec-crypto library */
	if (xmlSecCryptoInit() < 0) {
		oscap_seterr(OSCAP_EFAMILY_XML, "Xmlsec-crypto initialization failed.");
		return(-1);
	}

	/* find Signature node */
	xmlNodePtr root = xmlDocGetRootElement(doc);
	node = xmlSecFindNode(root, xmlSecNodeSignature, xmlSecDSigNs);
	if (node == NULL) {
		if (enforce_signature) {
			oscap_seterr(OSCAP_EFAMILY_XML, "Signature not found");
			goto cleanup;
		}
		/* Non-fatal error, datastreams without signatures are accepted */
		dI("Signature node not found");
		res = 0;
		goto cleanup;
	}

	/* Adds id attibutes of ds:component and ds:data-stream as general id
	 * attributes
	 */
	if (scap_type == OSCAP_DOCUMENT_SDS && _mark_sds_id_elements(doc) < 0) {
		goto cleanup;
	}

	/* create and initialize keys manager */
	mngr = xmlSecKeysMngrCreate();
	if (mngr == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XML, "failed to create keys manager");
		goto cleanup;
	}
	if (xmlSecCryptoAppDefaultKeysMngrInit(mngr) < 0) {
		oscap_seterr(OSCAP_EFAMILY_XML, "failed to initialize keys manager");
		goto cleanup;
	}

	/* create signature context */
	dsigCtx = xmlSecDSigCtxCreate(mngr);
	if (dsigCtx == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XML, "failed to create signature context");
		goto cleanup;
	}

	/* XMLSec 1.3 API Change: Enable KeyValue reading in the signature context */
	xmlSecKeyDataId kv_data_id = xmlSecKeyDataIdListFindByName(xmlSecKeyDataIdsGet(), BAD_CAST "key-value", xmlSecKeyDataUsageAny);
	xmlSecKeyDataId rsa_data_id = xmlSecKeyDataIdListFindByName(xmlSecKeyDataIdsGet(), BAD_CAST "rsa", xmlSecKeyDataUsageAny);

	res = xmlSecPtrListAdd(&(dsigCtx->keyInfoReadCtx.enabledKeyData), (const xmlSecPtr)kv_data_id);
	if (res < 0) {
		oscap_seterr(OSCAP_EFAMILY_XML, "failed to enable key data: key-value");
		goto cleanup;
	}
	res = xmlSecPtrListAdd(&(dsigCtx->keyInfoReadCtx.enabledKeyData), (const xmlSecPtr)rsa_data_id);
	if (res < 0) {
		oscap_seterr(OSCAP_EFAMILY_XML, "failed to enable key data: rsa");
		goto cleanup;
	}

	/* Verify signature */
	if (xmlSecDSigCtxVerify(dsigCtx, node) < 0) {
		oscap_seterr(OSCAP_EFAMILY_XML, "Signature verification failed");
		goto cleanup;
	}

	if (dsigCtx->status == xmlSecDSigStatusSucceeded) {
		dI("Signature is OK");
		res = 0;
	} else {
		dI("Signature is invalid.");
		res = 1;
	}

	/* compare good/bad references */
	int good = 0;
	xmlSecDSigReferenceCtxPtr dsigRefCtx;
	int size = xmlSecPtrListGetSize(&(dsigCtx->signedInfoReferences));
	for (int i = good = 0; i < size; i++) {
		dsigRefCtx = (xmlSecDSigReferenceCtxPtr)xmlSecPtrListGetItem(&(dsigCtx->signedInfoReferences), i);
		if(dsigRefCtx == NULL) {
			oscap_seterr(OSCAP_EFAMILY_XML, "Reference ctx is null");
			goto cleanup;
		}
		if (dsigRefCtx->status == xmlSecDSigStatusSucceeded)
			good++;
	}
	dI("SignedInfo references (ok/all): %d/%d", good, size);
	if (good != size) {
		res = 1;
	}

	/* compare good/bad manifests */
	size = xmlSecPtrListGetSize(&(dsigCtx->manifestReferences));
	for (int i = good = 0; i < size; i++) {
		dsigRefCtx = (xmlSecDSigReferenceCtxPtr)xmlSecPtrListGetItem(&(dsigCtx->manifestReferences), i);
		if (dsigRefCtx == NULL) {
			oscap_seterr(OSCAP_EFAMILY_XML, "Reference ctx is null");
			goto cleanup;
		}
		if (dsigRefCtx->status == xmlSecDSigStatusSucceeded)
			good++;
	}
	dI("Manifests references (ok/all): %d/%d", good, size);
	if (good != size) {
		res = 1;
	}
	if (res == 0) {
		printf("XML signature is valid.\n");
	}

cleanup:
	/* cleanup */
	if (dsigCtx != NULL)
		xmlSecDSigCtxDestroy(dsigCtx);

	/* destroy keys manager */
	if (mngr != NULL)
		xmlSecKeysMngrDestroy(mngr);

	/* Shutdown xmlsec-crypto library */
	xmlSecCryptoShutdown();

	/* Shutdown crypto library */
	xmlSecCryptoAppShutdown();

	/* Shutdown xmlsec library */
	xmlSecShutdown();

	/* Shutdown libxslt/libxml */
	xsltFreeSecurityPrefs(xsltSecPrefs);

	/* Unset the default XSLT security settings to prevent
	 * blockage of other oscap XSLT features (generate guide etc.)
	 */
	xsltSetDefaultSecurityPrefs(NULL);

	return res;
}

int oscap_signature_validate(struct oscap_source *source, struct oscap_signature_ctx *ctx, bool enforce_signature)
{
	int ret = 0;
	oscap_document_type_t scap_type = oscap_source_get_scap_type(source);
	const char *origin = oscap_source_readable_origin(source);

	if (scap_type == OSCAP_DOCUMENT_UNKNOWN) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unrecognized document type for: %s", origin);
		ret = -1;
	} else if (scap_type == OSCAP_DOCUMENT_SDS) {
		xmlDocPtr doc = oscap_source_get_xmlDoc(source);
		ret = _oscap_signature_validate_doc(doc, scap_type, ctx, enforce_signature);
	} else {
		const char *type_name = oscap_document_type_to_string(scap_type);
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unsupported document type %s for XML signature validation: %s", type_name, origin);
		ret = -1;
	}
	return ret;
}
