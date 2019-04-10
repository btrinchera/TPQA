@set INCLUDE=
@set LIB=
@set ARG1=%1
@shift
@set ARG2=%1
@shift
@set ARG3=%1
@shift
@set ARG4=%1
@shift
@set ARG5=%1
@shift
@set ARG6=%1
@shift
@set ARG7=%1
@shift
@set REST=%1 %2 %3 %4 %5 %6 %7 %8 %9
@echo Compiling %ARG1%
@"c:\program files (x86)\national instruments\cvi2013\bin\clang\2.9\clang.exe" -cc1 -nostdinc -emit-obj -add-plugin cvi-emit-defprots -plugin-arg-cvi-emit-defprots %ARG2%.cvidefprots -triple amd64-pc-win32 -fno-caret-diagnostics -fno-gnu-keywords -fms-extensions -mms-bitfields  -fmath-errno -mdisable-fp-elim -Wno-microsoft -Werror=gnu -Werror=pointer-arith -Wno-pointer-sign -Wno-unreachable-code -Wno-parentheses -Wno-unused-variable -Wuninitialized -Wno-div-by-zero -Wno-array-bounds -Wno-null-dereference -Wno-ignored-qualifiers -Wreturn-type -Wno-panel-handle -Wno-tautological-compare -Wno-empty-body -Wno-missing-braces -Wno-overlength-strings -Wno-cast-align -Wno-missing-noreturn -Wno-invalid-noreturn -Wno-shadow -Wno-switch -Wno-switch-enum -Wno-unused-function -Wno-unused-label -Wno-unused-parameter -Wno-unused-value -Wno-used-but-marked-unused -Wno-conversion -Wno-sign-compare -Wno-char-subscripts -Wno-declaration-after-statement -Wno-typecheck-convert -Wno-float-equal -Wno-vla -Wno-attributes -Wno-unknown-attributes -Wno-deprecated-declarations -Wno-packed -Wno-padded -Wno-comment -Wno-format -Wno-implicit-int -Wno-implicit-function-declaration -Wno-builtin-implicit-decl -Wno-missing-declarations -Wno-missing-field-initializers -Wno-trigraphs -Wno-multichar -Wno-long-long -Wno-unknown-pragmas -Wno-ignored-pragmas -Wno-pragma-reset -Wno-pragma-on-off-switch -Wno-pragma-unused -Wno-end-of-directive -Wno-pragma-not-supported -Wno-unexpected-token -Wno-four-char-constants -Wno-discard-qual -Wno-sign-conversion -Wno-variadic-macros -Wno-macro-name -Wno-macro-poisoning -Wno-macro-redefined -Wno-builtin-macro-redefined -Wno-builtin-macro-undefined -Wno-unused-macros -Wno-gnu-designator -Wno-nonnull -Wno-constant-conversion -Wno-out-of-scope-declarations -Wno-static-non-static -Wno-constant-too-large -Wno-#warnings -Wno-undef -Wno-include-next -Wno-extra-tokens -Wno-line-number -Wno-weak -Wno-member-redeclare -Wno-expected-semi -Wno-too-many-args -Wno-literal-range -Wno-unknown-escape -Wno-shift -Wno-char-init -Wno-braces-init -Wno-incomplete-field -Wno-tentative -Wno-ordered-comparison -Wno-pointer-compare -Wno-stack-memory -Wno-excess-elements -Wno-missing-terminating -Wno-illegal-storage -Wno-clang -Wno-backslash -Wno-decl-param -Wno-negative-to-unsigned -Wno-extern -Wno-constant-logical-operand -Wno-initializer-overrides -Wno-variadic-call-conv -Wno-main -Wno-omp-nested-parallel -Wno-omp-repeated-ordered -Wpointer-int-conv -Wint-conversion -Wpointer-conv -fdiagnostics-show-option  -cvi-debugging=standard -disable-current-directory -I "c:\TPQA with CMI Matlab Module\cvibuild.TPQA\Debug64\UnsavedChanges" -I %ARG4% %REST% -Werror=implicit-function-declaration -Wuninitialized-runtime -std=c99     -main-file-name %ARG7%  @"c:\TPQA with CMI Matlab Module\cvibuild.TPQA\Debug64\CVI_includes.txt" -D_CVI_="1300"  -D_NI_x64_="1"  -D_NI_mswin_="1"  -D_NI_mswin64_="1"  -D_CVI_DEBUG_="1"  -D_CVI_EXE_="1"  -D_LINK_CVIRTE_="1"  -D_CVI_FDS_="1"  -D_CVI_USE_FUNCS_FOR_VARS_="1"  -D__DEFALIGN="8"  -D_NI_VC_="220"  -D_WIN32="1"  -D_WIN64="1"  -DWIN32="1"  -DWIN64="1"  -D__WIN32__="1"  -D__WIN64__="1"  -D_WINDOWS="1"  -D__NT__="1"  -D_M_X64="1"  -D_M_AMD64="1"  -D__FLAT__="1"  -D_PUSHPOP_SUPPORTED="1"  -D_CVI_C99_EXTENSIONS_="1"  -D_CVI_PROFILE_LEVEL_="0"  -D_TARGET_FILE_VERSION_="\"%ARG5%\""  -D_TARGET_PRODUCT_VERSION_="\"%ARG6%\""  -D_OPENMP="200505"  -D_CVI_CONFIGURATION_NAME_="\"Debug64\""  -D_CVI_RTE_SHARED_="1"  -DWIN32_LEAN_AND_MEAN  -o %ARG2%  %ARG1%  > %ARG3% 2>&1
@if errorlevel 1 goto err
@echo Compile succeeded for %ARG1%
@exit 0
:err
@echo Compile failed for %ARG1%
@exit 1
