#include <arpa/inet.h>
#include <assert.h>
#include <utility>

#include "raw_decoder.h"

#include <algorithm> 
#include <fstream>
#include <iostream>
#include <utility>

#include <TH1F.h>
#include <TCanvas.h>
using namespace std;

//==========================================================================
RawDecoder::RawDecoder(unsigned int * buffer, int n)
{ 
  mAPVRawSingleEvent.clear();
  mAPVRawHisto.clear();

  fBuf = n;
  buf = new unsigned int[fBuf];
  for(int i=0;i<fBuf;i++)
  {
    buf[i] = buffer[i];
  }
  Decode();
}

//==========================================================================
RawDecoder::RawDecoder(const vector<uint32_t> &buffer, int start, int end)
{
  mAPVRawSingleEvent.clear();
  mAPVRawHisto.clear();

  fBuf = end - start;
  buf = new unsigned int[fBuf];
  int bufp = 0;
  for(int i=start;i<end;i++)
    {
      buf[bufp] = buffer[i];
      bufp++;
    }
  if(bufp!=fBuf){cout<<"vector passed and vector doesnt match"<<endl;}
  if(fBuf <= 0)
    {
      cout<<"empty vector passed in..."<<endl;
      return;
    }
};

//==========================================================================
RawDecoder::~RawDecoder()
{
  //free buf
  delete[] buf;

  //clear maps
  map<int, vector<int> >::iterator it;
  for(it=mAPVRawSingleEvent.begin(); it!=mAPVRawSingleEvent.end(); ++it)
  {
      it->second.clear();
  }
  mAPVRawSingleEvent.clear();

  //clear APV raw histos
  map<int, TH1F*>::iterator it_apv;
  for(it_apv=mAPVRawHisto.begin(); it_apv!=mAPVRawHisto.end(); ++it_apv)
  {
      TH1F *h = it_apv->second;
      h->Delete();
  }
  mAPVRawHisto.clear();
  //
}

//==========================================================================
map<int, vector<int> > RawDecoder::Decode()
{
  // cout<<"gethit"<<endl;
  map<int, vector<int> > mmHit;
  uint32_t data;
  int mpdid,apvid,stripNo;
  int wordCount, wordIndex;
  int adc[6];
  for(int i=3;i<fBuf;i++)//skipping ssp data
    {
      data = buf[i];
      if( (data>>30) == 0x2 ){ // if data defining word
	mpdid = data & 0x1f;
	cout<<"mpdID: "<<mpdid<<endl;
	
	wordCount = 0;
	continue;
      }else if(data>>24 == 0xf8){ // if filler word
	break;
      }
      
      wordIndex = wordCount%3;
      wordCount++;
      switch(wordIndex)
	{
	case 0:
	  stripNo = data>>26;
	  adc[0]  = data       & 0x1fff;
	  if(adc[0] >= 0x1000) adc[0] = (adc[0] & 0xfff) - 0x1000;
	  adc[1]  = (data>>13) & 0x1fff;
	  if(adc[1] >= 0x1000) adc[1] = (adc[1] & 0xfff) - 0x1000;
	  break;
	case 1:
	  stripNo |= ((data>>26) & 0x3) << 5; 
	  adc[2]  = data       & 0x1fff;
	  if(adc[2] >= 0x1000) adc[2] = (adc[2] & 0xfff) - 0x1000;
	  adc[3]  = (data>>13) & 0x1fff;
	  if(adc[3] >= 0x1000) adc[3] = (adc[3] & 0xfff) - 0x1000;
	  break;
	case 2:
	  apvid = data>>26;
	  adc[4]  = data       & 0x1fff;
	  if(adc[4] >= 0x1000) adc[4] = (adc[4] & 0xfff) - 0x1000;
	  adc[5]  = (data>>13) & 0x1fff;
	  if(adc[5] >= 0x1000) adc[5] = (adc[5] & 0xfff) - 0x1000;
	  break;
	}
	
      if(wordIndex==2){
	int hybridID = (mpdid<<12) | (apvid<<8) | stripNo;
	for(int i=0; i<6 ;i++)
	  { 
	    cout<<"adc_"<<i<<" : "<<adc[i]<<endl;
	    if(adc[i]>5000)
	      getchar();
	    mmHit[hybridID].push_back( adc[i] );
	  }
      }
    }
  return mmHit;
}

map<int, vector<int> > RawDecoder::GetHits(map<int,vector<int> > mMapping)
{
  // cout<<"gethit"<<endl;
  map<int, vector<int> > mmHit;
  uint32_t data;
  int mpdid,apvid,stripNo;
  int wordCount, wordIndex;
  int adc[6];
  for(int i=3;i<fBuf;i++)//skipping ssp data
    {
      data = buf[i];
      if( (data>>30) == 0x2 ){ // if data defining word
	mpdid = data & 0x1f;
	wordCount = 0;
	continue;
      }else if(data>>24 == 0xf8){ // if filler word
	break;
      }
      
      wordIndex = wordCount%3;
      wordCount++;
      switch(wordIndex)
	{
	case 0:
	  stripNo = data>>26;
	  adc[0]  = data       & 0x1fff;
	  if(adc[0] >= 0x1000) adc[0] = (adc[0] & 0xfff) - 0x1000;
	  adc[1]  = (data>>13) & 0x1fff;
	  if(adc[1] >= 0x1000) adc[1] = (adc[1] & 0xfff) - 0x1000;
	  break;
	case 1:
	  stripNo |= ((data>>26) & 0x3) << 5; 
	  adc[2]  = data       & 0x1fff;
	  if(adc[2] >= 0x1000) adc[2] = (adc[2] & 0xfff) - 0x1000;
	  adc[3]  = (data>>13) & 0x1fff;
	  if(adc[3] >= 0x1000) adc[3] = (adc[3] & 0xfff) - 0x1000;
	  break;
	case 2:
	  apvid = data>>26;
	  adc[4]  = data       & 0x1fff;
	  if(adc[4] >= 0x1000) adc[4] = (adc[4] & 0xfff) - 0x1000;
	  adc[5]  = (data>>13) & 0x1fff;
	  if(adc[5] >= 0x1000) adc[5] = (adc[5] & 0xfff) - 0x1000;
	  break;
	}
	
      if(wordIndex==2){
	//	cout<<mpdid<<" "<<apvid<<" "<<stripNo<<endl;
	int hybridID = (mpdid<<12) | (apvid<<8) ;
	int RstripPos;	  
	int RstripNb = ChNb[stripNo];
	RstripNb=RstripNb+(127-2*RstripNb)*mMapping[hybridID][3];       //re-matching for inverted strips Nb
	RstripPos=RstripNb+128*mMapping[hybridID][2];                   // calculate position
	int detID = mMapping[hybridID][0];
	int planeID = mMapping[hybridID][1];
	int HitHybridID = (detID<<13) | (planeID<<12) | RstripPos;
	for(int i=0; i<6 ;i++)
	  { 
	    mmHit[HitHybridID].push_back( adc[i]   );//if stop here, reduce 200ms/10k event
	  }
      }
    }
  // cout<<"endof Gethit"<<endl;
  return mmHit;
}




//==========================================================================
map<int, vector<int> > RawDecoder::GetDecoded()
{
  return mAPVRawSingleEvent;
}



void RawDecoder::DrawHits(map<int,vector<int> > mMapping, TCanvas *c)
{

  map<int,TH1F*> hitHisto;
  for(int i=0;i<10;i++)
    {
      //    hitHisto[i]->Delete();
      hitHisto[i]= new TH1F(Form("GEM_%d__plane_%d",i/2,i%2),Form("GEM_%d__plane_%d",i/2,i%2),1600,0,1600);
    }
   
  uint32_t data;
  int mpdid,apvid,stripNo;
  int wordCount, wordIndex;
  int adc[6];
  for(int i=3;i<fBuf;i++)//skipping ssp data
    {
      data = buf[i];
      if( (data>>30) == 0x2 ){ // if data defining word
	
	mpdid = data & 0x1f;
	cout<<mpdid<<endl;
	wordCount = 0;
	continue;
      }else if(data>>24 == 0xf8){ // if filler word
	break;
      }
      
      wordIndex = wordCount%3;
      wordCount++;
      switch(wordIndex)
	{
	case 0:
	  stripNo = data>>26;
	  adc[0]  = data       & 0x1fff;
	  if(adc[0] >= 0x1000) adc[0] = (adc[0] & 0xfff) - 0x1000;
	  adc[1]  = (data>>13) & 0x1fff;
	  if(adc[1] >= 0x1000) adc[1] = (adc[1] & 0xfff) - 0x1000;
	  break;
	case 1:
	  stripNo |= ((data>>26) & 0x3) << 5; 
	  adc[2]  = data       & 0x1fff;
	  if(adc[2] >= 0x1000) adc[2] = (adc[2] & 0xfff) - 0x1000;
	  adc[3]  = (data>>13) & 0x1fff;
	  if(adc[3] >= 0x1000) adc[3] = (adc[3] & 0xfff) - 0x1000;
	  break;
	case 2:
	  apvid = data>>26;
	  adc[4]  = data       & 0x1fff;
	  if(adc[4] >= 0x1000) adc[4] = (adc[4] & 0xfff) - 0x1000;
	  adc[5]  = (data>>13) & 0x1fff;
	  if(adc[5] >= 0x1000) adc[5] = (adc[5] & 0xfff) - 0x1000;
	  break;
	}
	
      if(wordIndex==2){
	//	cout<<mpdid<<" "<<apvid<<" "<<stripNo<<endl;
	int hybridID = (mpdid<<12) | (apvid<<8) ;
	int RstripPos;	  
	int RstripNb = ChNb[stripNo];
	RstripNb=RstripNb+(127-2*RstripNb)*mMapping[hybridID][3];       //re-matching for inverted strips Nb
	RstripPos=RstripNb+128*mMapping[hybridID][2];                   // calculate position
	int detID = mMapping[hybridID][0];
	int planeID = mMapping[hybridID][1];
	int HitHybridID = (detID<<13) | (planeID<<12) | RstripPos;
	int sum = 0;
	for(int i=0; i<6 ;i++)
	  { 
	    sum += adc[i];
	  }

	hitHisto[detID*2+planeID]->Fill(RstripPos,sum/6);
      }
    }
  
  for(int i=0;i<2;i++)
    {
      c->cd(i+1);
      hitHisto[i]->Draw();
    }
  c->Update();
  getchar();
    for(int i=0;i<10;i++)
    {
      hitHisto[i]->Delete();
    }

  return;
}
