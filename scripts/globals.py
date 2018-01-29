

LeptonicTop    = 'Tag1_Top1_lhood'
HadronicTop    = 'Tag1_Top2_lhood'
CombinedTop    = 'Tag1_TopLepHad_lhood'
SystemMass     = 'Tag1_SystemMass'
SystemPt       = 'Tag1_SystemPt'
SystemRapidity = 'Tag1_SystemRapidity'

sampleNames = [
               LeptonicTop,
               HadronicTop,
               SystemMass,
               SystemPt,
               SystemRapidity,
              ]

regValues   = {LeptonicTop    :4,
               HadronicTop    :4,
               CombinedTop    :4,
               SystemMass     :3,
               SystemPt       :3,
               SystemRapidity :4,
              }

ejets       = 'el'
mujets      = 'mu'
ljets       = 'co'
channels    = [
               ejets,
               mujets,
               ljets,
              ]
root_channels = {ejets :'e+jets',
                 mujets:'#mu+jets',
                 ljets :'l+jets',
                }

half0       = 'half0'
half1       = 'half1'
halves      = [
               half0,
               half1,
              ]
oppositeHalves = [
                  half1,
                  half0,
                 ]

binByBin    = 'BinByBin'
inversion   = 'Inversion'
SVD         = 'SVD'
Bayesian    = 'Bayesian'
unfoldingMethods = [
                    inversion,
                    binByBin,
                    SVD,
                    Bayesian,
                   ]

