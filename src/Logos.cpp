#include "Logos.h"

int Logos::estimateMatches(std::vector<Point*> vP1, std::vector<Point*> vP2, std::vector<PointPair*> &globalmatches){

matches.clear();

// for each point
int count1 = 0;

for (pit1=vP1.begin(); pit1<vP1.end(); ++pit1,count1++){
	(*pit1)->nearestNeighbours(vP1,count1,this->getNum1());
	int count2 = 0;
        // find possible matches
        for (pit2=vP2.begin(); pit2<vP2.end(); ++pit2,count2++){
		if ((*pit1)->getlabel()!= (*pit2)->getlabel()){
			continue;
		}
		// this is a possible match in Image 2
                // get nearest neighbours
	        (*pit2)->nearestNeighbours(vP2,count2,this->getNum2());

                PointPair* ptpr = new PointPair((*pit1),(*pit2));
		ptpr->addPositions(count1,count2); 
                pp.clear();
		ptpr->computeLocalSupport(pp,this->getNum2());
                
               // calc matches
               int support = 0;
	       for (it=pp.begin(); it<pp.end(); ++it){
		    Match* m = new Match(ptpr,(*it));
                    if (evaluateMatch(m)){
                           support++;	
                    } 
	       }
              if (support>0){
                ptpr->setsupport(support);
                matches.push_back(ptpr);
                updatebin(ptpr->getrelori());
              } 
	}
}

// do global orientation
double maxang = calcGlobalOrientation();
// find which matches are within global orientation limit
int numinliers=0;
globalmatches.clear();
for (it=matches.begin(); it!=matches.end(); ++it){
       if (std::abs((*it)->getrelori()-maxang)<GLOBALORILIMIT){
	     numinliers++;
             globalmatches.push_back(*it);
	}
}

return numinliers;

}

bool Logos::evaluateMatch(Match* m){
 return  ((m->getRelOrientation()<getIntraOriLimit()) && 
       (m->getRelScale()<getIntraScaleLimit()) && 
       (m->getInterOrientation()<getInterOriLimit()) && 
       (m->getInterScale()<getInterScaleLimit()));
}


void Logos::updatebin(double input){
   double binnumber=floor((input-LB)/BINSIZE);
   // compare binnumber to BINNUMBER
   if (binnumber<BINNUMBER){
	bins[(unsigned int)binnumber]++;
   } else {
        bins[(unsigned int)(BINNUMBER-1)]++;
   }
}

double Logos::calcGlobalOrientation(){
	// find max bin
	// check BINNUMBER is big enough
        if (BINNUMBER<3){
                return 0.0;
        }
	int* bins2 = new int[BINNUMBER];
	int maxval = 0;
	unsigned int maxix = 0;
	bins2[0] = bins[0]+bins[1]+bins[BINNUMBER-1];
	maxval = bins2[0];
	for (unsigned int i=1; i<BINNUMBER; i++){
	  if (i==BINNUMBER-1){
		bins2[i] = bins[i]+bins[i-1]+bins[0];
	  } else {
		  bins2[i] = bins[i]+bins[i-1]+bins[i+1];
	  } 
	  if (bins2[i]>maxval){
	    maxval = bins2[i];
	    maxix = i;
	  }
	}
      // convert to an angle 
      //return LB+((double)maxix)*BINSIZE;
      return LB+((double)maxix)*BINSIZE+BINSIZE/2;
}
