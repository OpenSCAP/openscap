oval53dir = $(pkgdatadir)/schemas/oval/5.3/
oval54dir = $(pkgdatadir)/schemas/oval/5.4/
oval55dir = $(pkgdatadir)/schemas/oval/5.5/
oval56dir = $(pkgdatadir)/schemas/oval/5.6/
oval57dir = $(pkgdatadir)/schemas/oval/5.7/
oval58dir = $(pkgdatadir)/schemas/oval/5.8/
oval59dir = $(pkgdatadir)/schemas/oval/5.9/
oval510dir = $(pkgdatadir)/schemas/oval/5.10/
oval5101dir = $(pkgdatadir)/schemas/oval/5.10.1/
oval511dir = $(pkgdatadir)/schemas/oval/5.11/
oval5111dir = $(pkgdatadir)/schemas/oval/5.11.1/
oval5112dir = $(pkgdatadir)/schemas/oval/5.11.2/
sce10dir = $(pkgdatadir)/schemas/sce/1.0/
xccdf11dir = $(pkgdatadir)/schemas/xccdf/1.1/
xccdf11tailoringdir = $(pkgdatadir)/schemas/xccdf/1.1-tailoring/
xccdf12dir = $(pkgdatadir)/schemas/xccdf/1.2/
sds12dir = $(pkgdatadir)/schemas/sds/1.2/
arf11dir = $(pkgdatadir)/schemas/arf/1.1/
ocil20dir = $(pkgdatadir)/schemas/ocil/2.0/
cpe20dir = $(pkgdatadir)/schemas/cpe/2.0/
cpe21dir = $(pkgdatadir)/schemas/cpe/2.1/
cpe22dir = $(pkgdatadir)/schemas/cpe/2.2/
cpe23dir = $(pkgdatadir)/schemas/cpe/2.3/
cvedir = $(pkgdatadir)/schemas/cve/
cvrfdir = $(pkgdatadir)/schemas/cvrf/
cvrf11dir = $(pkgdatadir)/schemas/cvrf/1.1/
commondir = $(pkgdatadir)/schemas/common/

oval53_DATA = $(wildcard oval/5.3/*.xsd oval/5.3/*.xsl)
oval54_DATA = $(wildcard oval/5.4/*.xsd oval/5.4/*.xsl)
oval55_DATA = $(wildcard oval/5.5/*.xsd oval/5.5/*.xsl)
oval56_DATA = $(wildcard oval/5.6/*.xsd oval/5.6/*.xsl)
oval57_DATA = $(wildcard oval/5.7/*.xsd oval/5.7/*.xsl)
oval58_DATA = $(wildcard oval/5.8/*.xsd oval/5.8/*.xsl)
oval59_DATA = $(wildcard oval/5.9/*.xsd oval/5.9/*.xsl)
oval510_DATA = $(wildcard oval/5.10/*.xsd oval/5.10/*.xsl)
oval5101_DATA = $(wildcard oval/5.10.1/*.xsd oval/5.10.1/*.xsl)
oval511_DATA = $(wildcard oval/5.11/*.xsd oval/5.11/*.xsl)
oval5111_DATA = $(wildcard oval/5.11.1/*.xsd oval/5.11.1/*.xsl)
oval5112_DATA = $(wildcard oval/5.11.2/*.xsd oval/5.11.2/*.xsl)

sce10_DATA = sce/1.0/sce-result-schema.xsd

xccdf11_DATA = $(wildcard xccdf/1.1/*.xsd xccdf/1.1/*.dtd)
xccdf11tailoring_DATA = $(wildcard xccdf/1.1-tailoring/*.xsd xccdf/1.1-tailoring/*.dtd)
xccdf12_DATA = $(wildcard xccdf/1.2/*.xsd xccdf/1.2/*.dtd xccdf/1.2/*.xsl)

sds12_DATA = $(wildcard sds/1.2/*.xsd sds/1.2/*.dtd)
arf11_DATA = $(wildcard arf/1.1/*.xsd)

ocil20_DATA = $(wildcard ocil/2.0/*.xsd sds/2.0/*.dtd)

cpe20_DATA = $(wildcard cpe/2.0/*.xsd cpe/2.0/*.dtd)
cpe21_DATA = $(wildcard cpe/2.1/*.xsd cpe/2.1/*.dtd)
cpe22_DATA = $(wildcard cpe/2.2/*.xsd cpe/2.2/*.dtd)
cpe23_DATA = $(wildcard cpe/2.3/*.xsd cpe/2.3/*.dtd)

cve_DATA = $(wildcard cve/*.xsd)

cvrf_DATA = $(wildcard cvrf/*.xsd)
cvrf11_DATA = $(wildcard cvrf/1.1/*.xsd)

common_DATA = $(wildcard common/*.xsd)

EXTRA_DIST = \
	$(oval53_DATA) \
	$(oval54_DATA) \
	$(oval55_DATA) \
	$(oval56_DATA) \
	$(oval57_DATA) \
	$(oval58_DATA) \
	$(oval59_DATA) \
	$(oval510_DATA) \
	$(oval5101_DATA) \
	$(oval511_DATA) \
	$(oval5111_DATA) \
	$(oval5112_DATA) \
	$(sce10_DATA) \
	$(xccdf11_DATA) \
	$(xccdf11tailoring_DATA) \
	$(xccdf12_DATA) \
	$(sds12_DATA) \
	$(arf11_DATA) \
	$(ocil20_DATA) \
	$(cpe20_DATA) \
	$(cpe21_DATA) \
	$(cpe22_DATA) \
	$(cpe23_DATA) \
	$(common_DATA) \
	$(cve_DATA) \
	$(cvrf_DATA) \
	$(cvrf11_DATA)

CLEANFILES = \
	build/*
