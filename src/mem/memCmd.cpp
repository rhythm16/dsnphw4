/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{
   cout << setw(15) << left << "MTReset: "
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
     // TODO
    try{

     vector<string> tokens;
     CmdExec::lexOptions(option, tokens, 0);
     if (tokens.size() == 1){ //numObjects
       int num;
       size_t num1 = num;
       if (myStr2Int(tokens[0], num)){
         num1 = num;
         mtest.newObjs(num1);
       }
       else {
         CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
       }
     }
     else if (tokens.size() == 3){ //plus -Array
       if (myStrNCmp("-Array", tokens[0], 2) == 0){ //first token is -Array
         int formerInt, latterInt;
         if (myStr2Int(tokens[1], formerInt) && myStr2Int(tokens[2], latterInt)){ //both are ints
           if (formerInt <= 0){
             CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
           }
           else {
             mtest.newArrs(latterInt, formerInt);
           }
         }
         else { //options not good
           CmdExec::errorOption(CMD_OPT_ILLEGAL, "");
         }
       }
       else if (myStrNCmp("-Array", tokens[1], 2) == 0){ //second token is -Array
         int formerInt, latterInt;
         if (myStr2Int(tokens[0], formerInt) && myStr2Int(tokens[2], latterInt)){ //both are ints
           if (latterInt <= 0){
             CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
           }
           else {
             mtest.newArrs(formerInt, latterInt);
           }
         }
         else {
           CmdExec::errorOption(CMD_OPT_ILLEGAL, "");
         }
       }
       else { //no -Array
         CmdExec::errorOption(CMD_OPT_ILLEGAL, "");
       }
     }
     else {
       CmdExec::errorOption(CMD_OPT_ILLEGAL, "");
     }

     return CMD_EXEC_DONE;
  }
  catch(bad_alloc){
    return CMD_EXEC_DONE;
  }
}



void
MTNewCmd::usage(ostream& os) const
{
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{
   cout << setw(15) << left << "MTNew: "
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> tokens;
   CmdExec::lexOptions(option, tokens, 0);
   //debug ===================
   //cout << "size of tokens is " << tokens.size() << endl;
   //for (int i = 0; i < tokens.size(); i++){
    //  cout << tokens[i] << endl;
   //}
   //debug ===================
   if (tokens.size() == 2){ //no array
     int objIdx, numOfRand;
     if (myStrNCmp("-Index", tokens[0], 2) == 0) {
       if (myStr2Int(tokens[1], objIdx)){ //good idx
         if (objIdx < 0){
           CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
         }
         else if (objIdx >= mtest.getObjListSize()){
           cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << objIdx << endl;
           CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
         }
         else {
           mtest.deleteObj(objIdx);
         }
       }
       else {
         CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
       }
     }
     else if (myStrNCmp("-Random", tokens[0], 2) == 0) {
       if (myStr2Int(tokens[1], numOfRand) && numOfRand > 0){
         //vector<int> randomIdx;
         if (mtest.getObjListSize() == 0){
           cerr << "Size of object list is 0!!" << endl;
         }
         else {
           for (int i = 0; i < numOfRand; i++){
             mtest.deleteObj(rnGen(mtest.getObjListSize()));
           }
         }
       }
       else {
         CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
       }
     }
     else {
       CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
     }
   }
   else if (tokens.size() == 3){ // with array
     int idx, numOfRands;
     if (myStrNCmp("-Array", tokens[0], 2) == 0){ //array at first
       if (myStrNCmp("-Index", tokens[1], 2) == 0){//index at second
         if (myStr2Int(tokens[2], idx)){//good int
           if (idx < 0){
             CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
           }
           else if (idx >= mtest.getArrListSize()){
             cerr << "Size of object list (" << mtest.getArrListSize() << ") is <= " << idx << endl;
             CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
           }
           else {
             mtest.deleteArr(idx);
           }
         }
         else {
           CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
         }
       }
       else if (myStrNCmp("-Random", tokens[1], 2) == 0){//random at second
         if (myStr2Int(tokens[2], numOfRands) && numOfRands > 0){
           if (mtest.getArrListSize() == 0){
             cerr << "Size of array list is 0!!" << endl;
           }
           else {
             for (int i = 0; i < numOfRands; i++){
               mtest.deleteArr(rnGen(mtest.getArrListSize()));
             }
           }
         }
         else {
           CmdExec::errorOption(CMD_OPT_ILLEGAL, "");
         }
       }
       else {
         CmdExec::errorOption(CMD_OPT_ILLEGAL, "");
       }
     }
     else if (myStrNCmp("-Array", tokens[2], 2) == 0){ //array at third
       if (myStrNCmp("-Index", tokens[0], 2) == 0){// index at first
         if (myStr2Int(tokens[1], idx)){//good int
           if (idx < 0){
             CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
           }
           else if (idx >= mtest.getArrListSize()){
             cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << idx << endl;
             CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
           }
           else {
             mtest.deleteArr(idx);
           }
         }
         else {
           CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
         }
       }
       else if (myStrNCmp("-Random", tokens[0], 2) == 0){//random at first
         if (myStr2Int(tokens[1], numOfRands) && numOfRands > 0){
           if (mtest.getArrListSize() == 0){
             cerr << "Size of array list is 0!!" << endl;
           }
           else {
             for (int i = 0; i < numOfRands; i++){
               mtest.deleteArr(rnGen(mtest.getArrListSize()));
             }
           }
         }
         else {
           CmdExec::errorOption(CMD_OPT_ILLEGAL, "");
         }
       }
       else {
         CmdExec::errorOption(CMD_OPT_ILLEGAL, "");
       }
     }
     else {
       CmdExec::errorOption(CMD_OPT_ILLEGAL, "");
     }
   }
   else {
     CmdExec::errorOption(CMD_OPT_ILLEGAL, "");
   }
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{
   cout << setw(15) << left << "MTDelete: "
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{
   cout << setw(15) << left << "MTPrint: "
        << "(memory test) print memory manager info" << endl;
}
