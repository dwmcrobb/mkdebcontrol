include Makefile.vars

OBJFILES    = DwmDebControlParser.o \
              DwmDebControlLexer.o \
              DwmDebPkgDepend.o \
              DwmDebPkgVersion.o \
              DwmDebVersionString.o \
              mkdebcontrol.o
OBJDEPS     = $(OBJFILES:%.o=deps/%_deps)
PKGTARGETS  = ${STAGING}${PREFIXDIR}/bin/mkdebcontrol \
              ${STAGING}${PREFIXDIR}/man/mkdebcontrol.1

mkdebcontrol: ${OBJFILES}
	${CXX} ${CXXFLAGS} ${LDFLAGS} -o $@ $^ ${OSLIBS}

package:: pkgprep
	if [ ! -d staging/DEBIAN ]; then mkdir staging/DEBIAN; fi
	./mkdebcontrol -r ./debcontrol -s staging > staging/DEBIAN/control
	dpkg-deb -b --root-owner-group staging
	dpkg-name -o staging.deb

pkgprep:: ${PKGTARGETS}

${STAGING}${PREFIXDIR}/bin/mkdebcontrol: mkdebcontrol
	./install-sh -s -c -m 555 $< $@

${STAGING}${PREFIXDIR}/man/mkdebcontrol.1: mkdebcontrol.1
	./install-sh -c -m 644 $< $@

DwmDebControlLexer.cc: DwmDebControlLexer.lex DwmDebControlParser.hh
	flex -o$@ $<

DwmDebControlParser.hh: DwmDebControlParser.cc

DwmDebControlParser.cc: DwmDebControlParser.y
	bison -d -o$@ $<

#  dependency rule
deps/%_deps: %.cc
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${INCS} -c $< | \
	 sed 's/\($*\)\.o[ :]*/\1.o $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include ${OBJDEPS}
endif
endif

%.o: %.cc deps/%_deps
	${CXX} ${CXXFLAGS} ${INCS} -c $< -o $@

distclean:: clean
	rm -f config.log config.status debcontrol Makefile.vars

clean::
	rm -Rf staging
	rm -f mkdebcontrol_*.deb mkdebcontrol ${OBJFILES} ${OBJDEPS}
	rm -f DwmDebControlLexer.cc DwmDebControlParser.hh \
	  DwmDebControlParser.cc
