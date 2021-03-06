/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.0 (the "NPL"); you may not use this file except in
 * compliance with the NPL.  You may obtain a copy of the NPL at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the NPL is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the NPL
 * for the specific language governing rights and limitations under the
 * NPL.
 *
 * The Initial Developer of this code under the NPL is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation.  All Rights
 * Reserved.
 */

#include "mcff2mcf.h"



int
parseNextMCFBlob(NET_StreamClass *stream, char* blob, int32 size)
{
  RDFFile f;
  int32 n, last, m;
  n = last = 0;

  f = (RDFFile)stream->data_object;
  if ((f == NULL) || (size < 0)) {
    return MK_INTERRUPTED;
  }

  while (n < size) {
    char c = blob[n];
    m = 0;
    memset(f->line, '\0', f->lineSize);
    if (f->holdOver[0] != '\0') {
      memcpy(f->line, f->holdOver, strlen(f->holdOver));
      m = strlen(f->holdOver);
      memset(f->holdOver, '\0', RDF_BUF_SIZE);
    }
    while ((m < f->lineSize) && (c != '\r') && (c != '\n') && (n < size)) {
      f->line[m] = c;
      m++;
      n++;
      c = blob[n];
    }
    n++;
    if (m > 0) {
      if ((c == '\n') || (c == '\r')) {
	last = n;
	parseNextMCFLine(f, f->line);
      } else if (size > last) {
	memcpy(f->holdOver, f->line, m);
      }
    }
  }
  return(size);
}



void
parseNextMCFLine (RDFFile f, char* line)
{
  char* nextToken ;
  int16 offset = 0;
  RDF_Error err;
  
  if ((nullp(line)) || (line[0] == '\0'))return; 
  nextToken = getMem(MAX_URL_SIZE);
  err = getFirstToken(line, nextToken, &offset);
  
  offset++; 
  if ((err != noRDFErr) &&  (nextToken[0] == ';')) {
    freeMem(nextToken);
    return;
  }
  if (startsWith("begin-headers", nextToken)) {
    f->status = HEADERS;
  } else if (startsWith("end-headers", nextToken)) {
    f->status = BODY;
  } else if (startsWith("unit", nextToken)) {
    f->status = BODY; 
    if (!(nullp(f->currentResource))) resourceTransition(f);
    getFirstToken(&line[offset], nextToken, &offset);
    f->currentResource = resolveReference(nextToken, f); 
  } else if (nextToken[strlen(nextToken)-1] == ':') {
    memset(f->currentSlot, '\0', 100);
    memcpy(f->currentSlot, nextToken, strlen(nextToken)-1);
    while (getFirstToken(&line[offset], nextToken, &offset) == noRDFErr) {
      if (f->status == HEADERS) {
	assignHeaderSlot(f, f->currentSlot, nextToken);
      } else if (f->currentResource) {
	assignSlot(f->currentResource, f->currentSlot, nextToken, f); 
      }
      offset++;
    }
  }
  freeMem(nextToken);
}



void
finishMCFParse (RDFFile f)
{
  parseNextMCFLine(f, f->storeAway);
  resourceTransition(f); 
} 



void
resourceTransition (RDFFile f)
{
  if ((f->currentResource)  && (!f->genlAdded))
    addSlotValue(f, f->currentResource, gCoreVocab->RDF_parent, f->rtop, 
		 RDF_RESOURCE_TYPE, true); 
  f->genlAdded = false;
}



void
assignHeaderSlot (RDFFile f, char* slot, char* value)
{
  if (startsWith(slot, "expiresOn")) {
    if (f->expiryTime == NULL)  f->expiryTime = (PRTime*)getMem(sizeof(PRTime));
    if (PR_ParseTimeString (value, 0, f->expiryTime) !=  PR_SUCCESS) {
      freeMem(f->expiryTime);
      f->expiryTime = NULL;
    }
  } else if (!(startsWith(slot, "RDFVersion"))) {
    assignSlot(f->top, slot, value, f);
  }	
}



RDF_Error
getFirstToken (char* line, char* nextToken, int16* l)
{
  PRBool in_paren = 0;
  PRBool in_string = 0;
  PRBool in_bracket = 0;
  PRBool something_seen = 0;
  uint16 front_spaces = 0;
  uint16 index;
  RDF_Error ans = -1;
  
  memset(nextToken, '\0', 200);
  
  for (index = 0; index < strlen(line); index++) {
    char c = line[index];
    
    if ((c == '\n') || (c == '\0') || (c == '\r')) {
      *l = index + *l  ;
      return ans;
    }
    
    if ((c == ':') && ((index == 0) || (line[index-1] == ' ')) && 
	((line[index+1] == ' ')	|| (line[index+1] == '\0'))) c = ' ';
    
    if (c != ' ') something_seen = 1;
    
    if (in_paren) {
      if (c == ')') {
	nextToken[index-front_spaces] = c;
	ans = noRDFErr;
	*l = index + *l ;
	return ans;
      } else {
	ans = noRDFErr;
	nextToken[index-front_spaces] = c;
      }
    } else if (in_string) {
      if (c == '"') {
	nextToken[index-front_spaces] = c;
	ans = noRDFErr;
	*l = index + *l ;
	return ans;
      } else {
	ans = noRDFErr;
	nextToken[index-front_spaces] = c;
      }
    } else if (in_bracket) {
      if (c == ']') {
	nextToken[index-front_spaces] = c; 
	*l = index + *l ;
	ans = noRDFErr;
	return ans;
      } else {
	ans = noRDFErr;
	nextToken[index-front_spaces] = c;
      }
    } else if (c == '"') {
      ans = noRDFErr;
      nextToken[index-front_spaces] = c;
      in_string = 1;
    } else if (c == '[') {
      ans = noRDFErr;
      nextToken[index-front_spaces] = c;
      in_bracket = 1;
    } else if (c == '(') {
      ans = noRDFErr;
      nextToken[index-front_spaces] = c;
      in_paren = 1;
    } else if (c == ' '){
      if (something_seen) { 
	*l = index + *l ;
	return ans;
      } else {
	front_spaces++;
      }
    } else {
      ans = noRDFErr;
      nextToken[index-front_spaces] = c;
    }
  }
  *l = index + *l ;
  return ans;
}	


void
addSlotValue (RDFFile f, RDF_Resource u, RDF_Resource s, void* v,
		RDF_ValueType type, PRBool tv)
{
   if (f == NULL || u == NULL || s == NULL || v == NULL)	return;
   if (s == gCoreVocab->RDF_child) {
     RDF_Resource temp = (RDF_Resource)v;
     if (type != RDF_RESOURCE_TYPE) return;
     s = gCoreVocab->RDF_parent;
     v = u;
     u = temp;
   }
   if ((s == gCoreVocab->RDF_parent) && (type == RDF_RESOURCE_TYPE)) {
     f->genlAdded = true; 
     if (strstr(resourceID(u), ".rdf") && startsWith("http", resourceID(u))) {
       RDFL rl = f->db->rdf;
       char* dburl = getBaseURL(resourceID(u));
       if (!startsWith(dburl, resourceID((RDF_Resource)v))) {
         while (rl) {
           RDF_AddDataSource(rl->rdf, dburl);
           rl = rl->next;
         }
         freeMem(dburl);
       }
     }
   }
   (*f->assert)(f, f->db, u, s, v, type, tv);
   if (s == gCoreVocab->RDF_parent) setContainerp((RDF_Resource)v, 1);
#ifndef MOZILLA_CLIENT
   notifySlotValueAdded(u, s, v, type);
#endif
}

void
assignSlot (RDF_Resource u, char* slot, char* value, RDFFile f)
{
  PRBool tv = true;
  if (value[0] == '(') {
    tv = false;
    value = &value[1];
    value[strlen(value)-1] = '\0';
  } 
  if (startsWith("default_genl", slot)) return;
  
  if (startsWith("name", slot) || (startsWith("local-name", slot))) { 
    value[strlen(value)-1] = '\0';
    addSlotValue(f, u, gCoreVocab->RDF_name, copyString(&value[1]), RDF_STRING_TYPE, tv);  
  }  else if (startsWith("specs", slot) || (startsWith("child", slot))) {
    RDF_Resource spec = resolveReference(value, f);  
    if (!nullp(spec)) addSlotValue(f, spec, gCoreVocab->RDF_parent, u, RDF_RESOURCE_TYPE, tv); 
  }  else if (startsWith("genls_pos", slot)) {
    RDF_Resource genl = resolveGenlPosReference(value, f);  
    if (!nullp(genl)) addSlotValue(f, u, gCoreVocab->RDF_parent, genl, RDF_RESOURCE_TYPE, tv);
  }  else if ((startsWith("genls", slot)) || (startsWith("parent", slot))) {
    RDF_Resource genl = resolveReference(value, f);
    if (!nullp(genl)) addSlotValue(f, u, gCoreVocab->RDF_parent, genl, RDF_RESOURCE_TYPE, tv);
  } else {
    void* parsed_value;
    RDF_ValueType data_type;
    RDF_Resource s = RDF_GetResource(NULL, slot, true);
    RDF_Error err = parseSlotValue(f, s, value, &parsed_value, &data_type); 
    if ((err == noRDFErr) && (!nullp(parsed_value)))   
      addSlotValue(f, u, s, parsed_value, data_type, tv);
  } 
}



RDF_Error
parseSlotValue (RDFFile f, RDF_Resource s, char* value, void** parsed_value, RDF_ValueType* data_type)
{
  if (value[0] == '"') {
    int32 size = strlen(value)-1;
    *parsed_value = getMem(size);
    value[size]  = '\0';
    *parsed_value = &value[1];
    *data_type = RDF_STRING_TYPE;
    return noRDFErr;
  } else if (value[0] == '#') {
    if (value[1] == '"') {
      value[strlen(value)-1] = '\0';
      value = &value[2];
    } else {
      value = &value[1];
    }
    *parsed_value = resolveReference(value, f);
    return noRDFErr;
  } else if (charSearch('.', value) == -1) {
    int16 ans = 0;
    /* XXX		sscanf(value, "%ld", &ans); */
    *data_type = RDF_INT_TYPE;
    return noRDFErr;
  } else {
    return -1;
  }
}



void
derelativizeURL (char* tok, char* url, RDFFile f)
{
  if ((tok[0] == '/') && (endsWith(".mco", tok))) {
	void stringAppendBase (char* dest, char* addition) ;
    stringAppendBase(url, f->url); 
    stringAppend(url, "#");
    stringAppend(url, tok);
  } else if  ((endsWith(".mco", tok)) && (charSearch('#', tok) == -1)) {
    void stringAppendBase (char* dest, char* addition) ;
    stringAppendBase(url, f->url);
    stringAppend(url, "#");
    stringAppend(url, tok);
  } else {
    memcpy(url, tok, strlen(tok));
  }
}



RDF_Resource
resolveReference (char *tok, RDFFile f)
{
  RDF_Resource existing;
  char* url = getMem(MAX_URL_SIZE);
  if (tok[0] == '#') tok = &tok[1];
  if (tok[strlen(tok)-1] == '"') tok[strlen(tok)-1] = '\0';
  if (tok[0] == '"') tok = &tok[1];
  memset(url, '\0', 200);
  if (charSearch(':', tok) == -1) { 
    derelativizeURL(tok, url, f);
  } else {
    memcpy(url, tok, strlen(tok));
  }
  if (strcmp(url,"this") == 0) {
    existing = f->top;
  } else {
    existing = RDF_GetResource(NULL, url, false);
  }
  if (existing != null) return existing;
  existing = RDF_GetResource(NULL, url, true);
  addToResourceList(f, existing);
  freeMem(url);
  return existing;
}



RDF_Resource
resolveGenlPosReference(char* tok,  RDFFile f)
{
  RDF_Resource ans;
  char* url = (char*)getMem(MAX_URL_SIZE);
  long i1, i2;
  i1 = charSearch('"', tok);
  i2 = revCharSearch('"', tok);
  memcpy(url, &tok[i1], i2-i1+1);
  ans = resolveReference(url, f);
  freeMem(url);
  return ans;
}



char *
getRelURL (RDF_Resource u, RDF_Resource top)
{
  char* uID = resourceID(u);
  char* topID = resourceID(top);
  if (startsWith(topID, uID)) {
    int16 n = charSearch('#', uID);
    if (n == -1) return uID;
    return &(uID)[n+1];
  } else return uID;
}
