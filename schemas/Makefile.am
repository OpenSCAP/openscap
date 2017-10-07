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
commondir = $(pkgdatadir)/schemas/common/

oval53_DATA = $(wildcard $(srcdir)/oval/5.3/*.xsd $(srcdir)/oval/5.3/*.xsl)
oval54_DATA = $(wildcard $(srcdir)/oval/5.4/*.xsd $(srcdir)/oval/5.4/*.xsl)
oval55_DATA = $(wildcard $(srcdir)/oval/5.5/*.xsd $(srcdir)/oval/5.5/*.xsl)
oval56_DATA = $(wildcard $(srcdir)/oval/5.6/*.xsd $(srcdir)/oval/5.6/*.xsl)
oval57_DATA = $(wildcard $(srcdir)/oval/5.7/*.xsd $(srcdir)/oval/5.7/*.xsl)
oval58_DATA = $(wildcard $(srcdir)/oval/5.8/*.xsd $(srcdir)/oval/5.8/*.xsl)
oval59_DATA = $(wildcard $(srcdir)/oval/5.9/*.xsd $(srcdir)/oval/5.9/*.xsl)
oval510_DATA = $(wildcard $(srcdir)/oval/5.10/*.xsd $(srcdir)/oval/5.10/*.xsl)
oval5101_DATA = $(wildcard $(srcdir)/oval/5.10.1/*.xsd $(srcdir)/oval/5.10.1/*.xsl)
oval511_DATA = $(wildcard $(srcdir)/oval/5.11/*.xsd $(srcdir)/oval/5.11/*.xsl)
oval5111_DATA = $(wildcard $(srcdir)/oval/5.11.1/*.xsd $(srcdir)/oval/5.11.1/*.xsl)
oval5112_DATA = $(wildcard $(srcdir)/oval/5.11.2/*.xsd $(srcdir)/oval/5.11.2/*.xsl)

sce10_DATA = sce/1.0/sce-result-schema.xsd

xccdf11_DATA = $(wildcard $(srcdir)/xccdf/1.1/*.xsd $(srcdir)/xccdf/1.1/*.dtd)
xccdf11tailoring_DATA = $(wildcard $(srcdir)/xccdf/1.1-tailoring/*.xsd $(srcdir)/xccdf/1.1-tailoring/*.dtd)
xccdf12_DATA = $(wildcard $(srcdir)/xccdf/1.2/*.xsd $(srcdir)/xccdf/1.2/*.dtd $(srcdir)/xccdf/1.2/*.xsl)

sds12_DATA = $(wildcard $(srcdir)/sds/1.2/*.xsd $(srcdir)/sds/1.2/*.dtd)
arf11_DATA = $(wildcard $(srcdir)/arf/1.1/*.xsd)

ocil20_DATA = $(wildcard $(srcdir)/ocil/2.0/*.xsd $(srcdir)/sds/2.0/*.dtd)

cpe20_DATA = $(wildcard $(srcdir)/cpe/2.0/*.xsd $(srcdir)/cpe/2.0/*.dtd)
cpe21_DATA = $(wildcard $(srcdir)/cpe/2.1/*.xsd $(srcdir)/cpe/2.1/*.dtd)
cpe22_DATA = $(wildcard $(srcdir)/cpe/2.2/*.xsd $(srcdir)/cpe/2.2/*.dtd)
cpe23_DATA = $(wildcard $(srcdir)/cpe/2.3/*.xsd $(srcdir)/cpe/2.3/*.dtd)

cve_DATA = $(wildcard $(srcdir)/cve/*.xsd)
common_DATA = $(wildcard $(srcdir)/common/*.xsd)

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
	$(cve_DATA)

CLEANFILES = \
	build/*
