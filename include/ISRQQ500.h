#ifndef ISRQQ500_h
#define ISRQQ500_h 1

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>
#include "marlin/VerbosityLevels.h"

using namespace lcio ;
using namespace marlin ;


/**  Example processor for marlin.
 * 
 *  If compiled with MARLIN_USE_AIDA 
 *  it creates a histogram (cloud) of the MCParticle energies.
 * 
 *  <h4>Input - Prerequisites</h4>
 *  Needs the collection of MCParticles.
 *
 *  <h4>Output</h4> 
 *  A histogram.
 * 
 * @param CollectionName Name of the MCParticle collection
 * 
 * @author F. Gaede, DESY
 * @version $Id: ISRQQ500.h,v 1.4 2005-10-11 12:57:39 gaede Exp $ 
 */

class ISRQQ500 : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new ISRQQ500 ; }
    
  ISRQQ500() ;
  
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() ;
  
  /** Called for every run.
   */
  virtual void processRunHeader( LCRunHeader* run ) ;
  
  /** Called for every event - the working horse.
   */
  virtual void processEvent( LCEvent * evt ) ; 
  
  
  virtual void check( LCEvent * evt ) ; 
  
  
  /** Called after data processing for clean up.
   */
  virtual void end() ;
  
  
 protected:
  float _ISR_CUT;
  std::string _DesiredFlavour ; 
  /** Input collection name.
   */
  std::string _colName{} ;

  int _nRun{} ;
  int _nEvt{} ;
} ;

#endif



