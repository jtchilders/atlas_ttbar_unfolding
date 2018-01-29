
# ROOT Stuff
ROOTC = $(shell root-config --cflags)
ROOTLIB = $(shell root-config --libs)

# RooUnfold stuff (may be user dependent)
UNFOLDING_PATH = $(PWD)
ROOUNFOLD_PATH=RooUnfold
ROOUNF_INCPATH=$(ROOUNFOLD_PATH)/src

COPTS=$(ROOTC) -I$(ROOUNF_INCPATH) -Iinc -c -g -Itclap/include -fPIC
LOPTS=$(ROOTLIB)

INCPATH=inc
SRCPATH=src
BINPATH=bin
OBJPATH=obj

OBJ=$(OBJPATH)/ttbarUnfold.o $(OBJPATH)/FileList.o $(OBJPATH)/AtlasStyle.o \
	$(OBJPATH)/ttbarUnfoldAnalysis.o $(OBJPATH)/FileHandler.o \
	$(OBJPATH)/Th1PlusLegend.o  $(OBJPATH)/globals.o $(OBJPATH)/MCFMHisto.o \
	$(OBJPATH)/TexTable.o $(OBJPATH)/SystTable.o $(OBJPATH)/SystematicList.o \
	$(OBJPATH)/KidonakisNNLO.o $(OBJPATH)/McAtNloBeforeCuts.o \
	$(OBJPATH)/AlpgenMC.o $(OBJPATH)/CombSystTable.o $(OBJPATH)/KinematicVariable.o \
	$(OBJPATH)/SystematicTextFile.o $(OBJPATH)/BLUECombination.o \
	$(OBJPATH)/AsymBLUECombination.o $(OBJPATH)/GeneratorLabel.o \
	$(OBJPATH)/Th1PlotAndRatio.o $(OBJPATH)/TopDecayChannel.o \
	$(OBJPATH)/PdfSystematic.o $(OBJPATH)/PlotLabel.o $(OBJPATH)/YangPecjakNNLO.o \
	$(OBJPATH)/SystematicBin.o $(OBJPATH)/PValueList.o \
	$(OBJPATH)/PValueTable.o $(OBJPATH)/PowHegMC.o \
	$(OBJPATH)/ToyCovariance.o $(OBJPATH)/MasterSystList.o \
	$(OBJPATH)/MatrixTable.o $(OBJPATH)/SampleDef.o \
	$(OBJPATH)/LatexDoc.o $(OBJPATH)/MCFMWithPDF.o $(OBJPATH)/Utils.o

ROO_OBJ=$(OBJPATH)/RooUnfoldBayes.o $(OBJPATH)/RooUnfoldBinByBin.o \
	$(OBJPATH)/RooUnfoldErrors.o $(OBJPATH)/RooUnfold.o \
	$(OBJPATH)/RooUnfoldResponse.o $(OBJPATH)/RooUnfoldSvd.o \
	$(OBJPATH)/RooUnfoldInvert.o $(OBJPATH)/RooUnfoldTUnfold.o \
	$(OBJPATH)/RooUnfoldParms.o $(OBJPATH)/RooUnfoldDict.o 

BIN1=exampleUnfolding
BIN2=unfoldingMethodsTest
BIN3=ensemblesTest
BIN4=toyRegScan
BIN5=regularization
BIN6=migrationToys
BIN7=compSamples
BIN8=compToys
BIN9=combineSystematics
BIN10=combineChannels
BIN11=createCorrelationMatrices
BIN_LIST=$(BIN1) $(BIN2) $(BIN3) $(BIN4) $(BIN5) $(BIN6) $(BIN7) $(BIN8) $(BIN9) $(BIN10) $(BIN11)
BIN=$(addprefix $(BINPATH)/,$(BIN_LIST))
BINOBJ=$(addprefix $(OBJPATH)/, $(addsuffix .o,$(BIN_LIST) ) )

SYSTHEADER=inc/SystematicSample.h

GCC=g++

target:  $(SYSTHEADER) $(OBJ) $(BINOBJ) $(ROO_OBJ) $(BIN)

$(BINPATH)/%: $(OBJPATH)/%.o $(OBJ) $(ROO_OBJ)
	@echo " "
	@echo "--------->>>>>> Linking" $*
	$(GCC) $(LOPTS) $(OBJ) $(ROO_OBJ) $(OBJPATH)/$*.o -o $(BINPATH)/$*
	@echo "----->>> Done."


# Build binary Source
$(OBJPATH)/%.o: $(SRCPATH)/%.cxx
	@echo " "
	@echo "--------->>>>>> Compiling" $*.o
	$(GCC) $(COPTS) $(SRCPATH)/$*.cxx -o $(OBJPATH)/$*.o
	@echo "----->>> Done."

# Build RooUnfold Source
$(OBJPATH)/%.o: $(ROOUNF_INCPATH)/%.cxx
	@echo " "
	@echo "--------->>>>>> Compiling" $*.o
	$(GCC) $(COPTS) $(ROOUNF_INCPATH)/$*.cxx -o $(OBJPATH)/$*.o
	@echo "----->>> Done."

# Build Generic Object Files
$(OBJPATH)/%.o: $(SRCPATH)/%.cpp $(INCPATH)/%.h
	@echo " "
	@echo "--------->>>>>> Compiling" $*.o
	$(GCC) $(COPTS) $(SRCPATH)/$*.cpp -o $(OBJPATH)/$*.o
	@echo "----->>> Done."

# build SystematicSample.h if it doesn't already exist
$(SYSTHEADER): scripts/makeSystematicSample.py
	@echo " "
	@echo "--------->>>>>> Making" $@
	$<
	@echo "----->>> Done."

# build RooUnfold Dictionary
$(ROOUNF_INCPATH)/RooUnfoldDict.cxx: $(ROOUNF_INCPATH)/RooUnfold_LinkDef.h
	@echo " "
	@echo "--------->>>>>> Making" $@
	cd $(ROOUNF_INCPATH)
	rootcint -f $(ROOUNF_INCPATH)/RooUnfoldDict.cxx -c -p -I$(ROOUNF_INCPATH) RooUnfoldBayes.h RooUnfold.h RooUnfoldResponse.h RooUnfoldTUnfold.h RooUnfoldBinByBin.h RooUnfoldErrors.h RooUnfoldInvert.h RooUnfoldParms.h RooUnfoldSvd.h RooUnfold_LinkDef.h
	cd $(UNFOLDING_PATH)
	@echo "----->>> Done."


clean:
	@echo " "
	@echo "--------->>>>>> Cleaning"
	rm -f $(OBJ) $(BINOBJ) $(BINPATH)/* *~ */*~ $(ROOUNF_INCPATH)/RooUnfoldDict.* $(ROOUNF_INCPATH)/*~
	@echo "----->>> Done."

rooclean:
	@echo " "
	@echo "--------->>>>>> Cleaning"
	rm -f $(ROO_OBJ) $(ROOUNF_INCPATH)/RooUnfoldDict.* $(ROOUNF_INCPATH)/*~
	@echo "----->>> Done."


