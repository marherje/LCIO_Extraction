#include "marlin/TestProcessor.h"
#include "marlin/Exceptions.h"
#include "ISRQQ500.h"
#include <iostream>

#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"


using namespace lcio ;
using namespace marlin ;


ISRQQ500 aISRQQ500 ;


ISRQQ500::ISRQQ500() : Processor("ISRQQ500") {

    // modify processor description
    _description = "ISRQQ500 extract the QQ signal from a sample with QQ+ISR (500 GeV)" ;
    // My inputs parameters?

    // register steering parameters: name, description, class-variable, default value

    registerProcessorParameter( "DesiredFlavour",
                                "This selects the quark flavour (b_quark, c_quark_ light_quark, none) ",
                                _DesiredFlavour,
                                std::string("b_quark"));

    registerProcessorParameter( "ISR_CUT",
                                "This defines the cut in GeV for signal/ISR ",
                                _ISR_CUT,
                                float(-1));

    registerInputCollection( LCIO::MCPARTICLE,
            "CollectionName" , 
            "Name of the MCParticle collection"  ,
            _colName ,
            std::string("MCParticlesSkimmed")
    );
}



void ISRQQ500::init() { 
    streamlog_out(DEBUG) << "   init called  " << std::endl ;
    // usually a good idea to
    printParameters() ;
    _nRun = 0 ;
    _nEvt = 0 ;
}


void ISRQQ500::processRunHeader( LCRunHeader* run) { 
    _nRun++ ;
} 



void ISRQQ500::processEvent( LCEvent * evt) { 
    // this gets called for every event 
    // usually the working horse ...

    // try to get lcio collection (exits if collection is not available)
    // NOTE: if the AIDAProcessor is activated in your steering file and Marlin is linked with
    //      RAIDA you may get the message: "*** Break *** segmentation violation" followed by a
    //      stack-trace (generated by ROOT) in case the collection is unavailable. This happens
    //      because ROOT is somehow catching the exit signal commonly used to exit a program
    //      intentionally. Sorry if this messsage may confuse you. Please ignore it!
    //LCCollection* col = evt->getCollection( _colName ) ;
    //MCParticle* p = dynamic_cast<MCParticle*>( col->getElementAt( i ) ) ;
    
    // Alternativelly if you do not want Marlin to exit in case of a non-existing collection
    // use the following (commented out) code:
    LCCollection* col = NULL;
    try{
        col = evt->getCollection( _colName );
    }
    catch( lcio::DataNotAvailableException e )
    {
        streamlog_out(WARNING) << _colName << " collection not available" << std::endl;
        col = NULL;
    }

    // this will only be entered if the collection is available

    if(_nEvt%2000==0){
      streamlog_out(DEBUG) << "Target flavour:" << _DesiredFlavour << std::endl;
    }
    
    // Selecting the first quark that appears:
    MCParticle* firstQ;
    signed int QFlavour = 0;
    for(int i=0;i<12;i++){
      MCParticle* ParticleScan=dynamic_cast<MCParticle*>(col->getElementAt(i));
      signed int ParticleScanType=ParticleScan->getPDG();      
      if( abs(ParticleScanType) < 6){
	QFlavour = abs(ParticleScanType);
	break;    
      }
    }
    
    if(_nEvt%2000==0){
      streamlog_out(DEBUG) << "Scanned PDG:" << QFlavour << std::endl;
    }
    
    if(_DesiredFlavour == "d_quark"){
      if(QFlavour != 1) throw marlin::SkipEventException(this);}
    else if(_DesiredFlavour == "u_quark"){
      if(QFlavour != 2) throw marlin::SkipEventException(this);}
    else if(_DesiredFlavour == "s_quark"){
      if(QFlavour != 3) throw marlin::SkipEventException(this);}
    else if(_DesiredFlavour == "c_quark"){
      if(QFlavour != 4) throw marlin::SkipEventException(this);}
    else if(_DesiredFlavour == "b_quark"){
      if(QFlavour != 5) throw marlin::SkipEventException(this);}
    else if(_DesiredFlavour == "light_quark"){
      if(QFlavour > 3) throw marlin::SkipEventException(this);
    }

    // Now we select the first two photon that appears, in case we decide to extract only the qqbar signals by doing a cut in the energy (ISR_CUT):
    float Photon1Energy;
    float Photon2Energy;
    int Photon1Index;
    if(_ISR_CUT == -1){
      //cout<<"not cutting in ISR"<<endl;
    }
    else{
      for(int i=0;i<10;i++){
	MCParticle* Photon1=dynamic_cast<MCParticle*>(col->getElementAt(i));
	if( Photon1->getPDG()==22 && Photon1->getGeneratorStatus()==1){
	  Photon1Energy=Photon1->getEnergy();
	  Photon1Index=i;
	  break;
	}
      }
      for(int a=Photon1Index+1;a<10;a++){
	MCParticle* Photon2=dynamic_cast<MCParticle*>(col->getElementAt(a));
	if( Photon2->getPDG()==22 && Photon2->getGeneratorStatus()==1){
	  Photon2Energy=Photon2->getEnergy();
	  break;
	}
      }
      float ISR_energy=Photon1Energy+Photon2Energy;
      
      if(_nEvt%2000==0){
	streamlog_out(DEBUG) << "K_reco" << ISR_energy << std::endl;
      }

      if(ISR_energy > _ISR_CUT){throw marlin::SkipEventException(this);}
    }

    

    //-- note: this will not be printed if compiled w/o MARLINDEBUG=1 !

    if(_nEvt%100==0){
    streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber()
			 << "   in run:  " << evt->getRunNumber() << std::endl ;
    }
    _nEvt ++ ;
}



void ISRQQ500::check( LCEvent * evt ) { 
    // nothing to check here - could be used to fill checkplots in reconstruction processor
}


void ISRQQ500::end(){ 

    //   std::cout << "ISRQQ500::end()  " << name() 
    // 	    << " processed " << _nEvt << " events in " << _nRun << " runs "
    // 	    << std::endl ;

}

