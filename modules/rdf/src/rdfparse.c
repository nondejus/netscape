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

/* 
   This file implements parsing support for the rdf data model.
   For more information on this file, contact rjc or guha 
   For more information on RDF, look at the RDF section of www.mozilla.org
*/

#include "rdfparse.h"
#include "mcf.h"
#include "mcff2mcf.h"

#define wsCharp(c) ((c == '\r') || (c == '\t') || (c == ' ') || (c == '\n'))



char
decodeEntityRef (char* string, int32* stringIndexPtr, int32 len)
{
  if (startsWith("lt;", string)) {
    *stringIndexPtr = *stringIndexPtr + 3;
    return '<';
  } else if (startsWith("gt;", string)) {
    *stringIndexPtr = *stringIndexPtr + 3;
    return '>';
  } else  if (startsWith("amp;", string)) {
    *stringIndexPtr = *stringIndexPtr + 4;
    return '&';
  } else return -1;
}



char *
copyStringIgnoreWhiteSpace(char* string)
{
   int32 len = strlen(string);
   char* buf = (char*)getMem(len + 1);
   PRBool inWhiteSpace = 1;
   int32 buffIndex = 0;
   int32 stringIndex = 0;

   while (stringIndex < len) {
     char nextChar = *(string + stringIndex);
     PRBool wsp = wsCharp(nextChar);
     if (!wsp) {
       if (nextChar == '&') {
         *(buf + buffIndex++) = decodeEntityRef(&string[stringIndex+1], 
                                                &stringIndex, len-stringIndex);
       } else {
         *(buf + buffIndex++) = nextChar;
       }
       inWhiteSpace = 0;
     } else if (!inWhiteSpace) {
       *(buf + buffIndex++) = ' ';
       inWhiteSpace = 1;
     } else {
       inWhiteSpace = 1;
     }
     stringIndex++;
   }

   return buf;
}



char *
getHref(char** attlist)
{
	char* ans = getAttributeValue(attlist, "rdf:href");
	if (!ans) ans = getAttributeValue(attlist, "RDF:href"); 
	if (!ans) ans = getAttributeValue(attlist, "href");
	return ans;
}
 

int
parseNextRDFXMLBlob (NET_StreamClass *stream, char* blob, int32 size)
{
  RDFFile f;
  int32 n, last, m;
  PRBool somethingseenp = 0;
  n = last = 0;

  f = (RDFFile)stream->data_object;
  if ((f == NULL) || (size < 0)) {
    return MK_INTERRUPTED;
  }

  while (n < size) {
    char c = blob[n];
    m = 0;
    somethingseenp = 0;
    memset(f->line, '\0', RDF_BUF_SIZE-1);
    if (f->holdOver[0] != '\0') {
      memcpy(f->line, f->holdOver, strlen(f->holdOver));
      m = strlen(f->holdOver);
      somethingseenp = 1;
      memset(f->holdOver, '\0', RDF_BUF_SIZE-1);
    }   
    while ((n < size) && (wsc(c))) {c = blob[++n];}
  /*  f->line[m++] = c;
    c = blob[++n]; */
    while ((m < RDF_BUF_SIZE) && (c != '<') && (c != '>')) {
      f->line[m] = c;
      m++;
      somethingseenp = (somethingseenp || (!(wsc(c))));
      n++;	
      if (n < size) c = blob[n]; 
      else break;
    }
    if (c == '>') f->line[m] = c;
    n++;
    if (m > 0) {
      if ((c == '<') || (c == '>')) {
	last = n;
	if (c == '<') f->holdOver[0] = '<'; 
	if (somethingseenp == 1) parseNextRDFToken(f, f->line);
      } else if (size > last) {
	memcpy(f->holdOver, f->line, m);
      }
    } else if (c == '<') f->holdOver[0] = '<';
  }
  return(size);
}



void
parseRDFProcessingInstruction (RDFFile f, char* token)
{
  char* attlist[2*MAX_ATTRIBUTES+1];
  char* elementName;  
  tokenizeElement(token, attlist, &elementName);
  if (strcmp(elementName, "?xml:namespace") == 0) {
    char* as = getAttributeValue(attlist, "prefix");
    char* url = getAttributeValue(attlist, "ns");
    if ((as != NULL) && (url != NULL)) {
      XMLNameSpace ns = (XMLNameSpace)getMem(sizeof(struct XMLNameSpaceStruct));
      size_t urln = strlen(url);
      XP_Bool addSlash = (url[urln-1] != '/');
      if(addSlash) urln++;
      ns->url = (char*)getMem(sizeof(char) * (urln + 1));
      sprintf(ns->url, "%s%s", url, addSlash ? "/" : "");
      ns->as = XP_STRDUP(as);
      ns->next = f->namespaces;
      f->namespaces = ns;
    }
  }  
} 



void
freeNamespaces (RDFFile f)
{
  XMLNameSpace ns1 = f->namespaces;
  while (ns1) {
    XMLNameSpace next = ns1->next;
    freeMem(ns1->as);
    freeMem(ns1->url);
    freeMem(ns1);
    ns1 = next;
  }
}



PR_PUBLIC_API(char *)
getAttributeValue (char** attlist, char* elName)
{
  size_t n = 0;
  if (!attlist) return NULL;
  while ((n < 2*MAX_ATTRIBUTES) && (*(attlist + n) != NULL)) {
    if (strcmp(*(attlist + n), elName) == 0) return *(attlist + n + 1);
    n = n + 2;
  }
  return NULL;
}



PRBool
tagEquals (RDFFile f, char* tag1, char* tag2)
{
	return (strcmp(tag1, tag2) == 0);
}



void
addElementProps (char** attlist, char* elementName, RDFFile f, RDF_Resource obj)
{
  uint32 count = 0;
  while (count < 2*MAX_ATTRIBUTES) {
    char* attName = attlist[count++];
    char* attValue = attlist[count++];
    if ((attName == NULL) || (attValue == NULL)) break;
    if (!tagEquals(f, attName, "href") && !tagEquals(f, attName, "rdf:href")  && !tagEquals(f, attName, "RDF:href") 
        && !tagEquals(f, attName, "id")) {
      addSlotValue(f, obj, ResourceFromElementName(f, attName), copyStringIgnoreWhiteSpace(attValue), 
		   RDF_STRING_TYPE, 1);
    }
  }
}



PRBool
knownObjectElement (char* eln)
{
	return (strcmp(eln, "RDF:Description") == 0);
}



char *
possiblyMakeAbsolute (RDFFile f, char* url)
{
	if (strchr(url, ':') != NULL) {
    return copyString(url);
  } else {
    char* ans = getMem(strlen(f->url) + strlen(url)+2);
    sprintf(ans, "%s#%s", f->url, url);
    return ans;
  }
}



PRBool
containerTagp (RDFFile f, char* elementName)
{
 return (tagEquals(f, elementName, "Container") || 
	    tagEquals(f, elementName, "Topic") ||
	    (tagEquals(f, elementName, "RelatedLinks")));
}

#define DC_TITLE "http://purl.org/metadata/dublin_core/title"
#define SM_CHILD "http://purl.org/metadata/sitemap/child"

RDF_Resource
ResourceFromElementName (RDFFile f, char* elementName)
{
  if(!elementName || (strchr(elementName, ':') == NULL) ) {
    return RDF_GetResource(NULL, elementName, 1);
  } else {
    XMLNameSpace ns = f->namespaces;
    while (ns) {
      if (startsWith(ns->as, elementName)) {
        RDF_Resource ans;
        size_t asn =  strlen(ns->as);
        size_t urln = strlen(ns->url);
        char* url = getMem(strlen(ns->url) + strlen(elementName)-asn);
        memcpy(url, ns->url, urln);
        strcat(url, &elementName[asn+1]);
		if (strcmp(url, DC_TITLE) == 0) {
          ans = gCoreVocab->RDF_name;
        } else if (strcmp(url, SM_CHILD) == 0) {
          ans = gCoreVocab->RDF_child;
        } else  
          ans = RDF_GetResource(NULL, url, 1);
        freeMem(url);
        return ans;
      }
      ns = ns->next;
    }    
    return RDF_GetResource(NULL, elementName, 1);
  }
}
        


void
parseNextRDFToken (RDFFile f, char* token)
{
  if (token[0] != '<')   {
    if ((f->status == EXPECTING_OBJECT) && (f->depth > 1)) {
      RDF_Resource u = f->stack[f->depth-2];
      RDF_Resource s = f->stack[f->depth-1];
      addSlotValue(f, u, s, copyStringIgnoreWhiteSpace(token), RDF_STRING_TYPE, 1);
    } 
  } else if  (startsWith("<!--", token)) {
    return;
  } else if (token[1] == '?')  {
    parseRDFProcessingInstruction(f, token);
  } else if (token[1] == '/') {
    if ((f->status != EXPECTING_OBJECT) && (f->status != EXPECTING_PROPERTY)) return;
    if (f->depth > 0) f->depth--;
    f->status = (f->status == EXPECTING_OBJECT ? EXPECTING_PROPERTY : EXPECTING_OBJECT);
  } else if ((f->status == 0) && startsWith("<RDF:RDF>", token)) {
      f->status = EXPECTING_OBJECT;
  } else if (startsWith("<RelatedLinks", token)) {
	f->stack[f->depth++] = f->top;
	f->status = EXPECTING_PROPERTY;
  } else {
    PRBool emptyElementp = (token[strlen(token)-2] == '/');
    char* attlist[2*MAX_ATTRIBUTES+1];
    char* elementName;
    if ((f->status != EXPECTING_OBJECT) && (f->status != EXPECTING_PROPERTY)) return;
    tokenizeElement(token, attlist, &elementName);
    if ((f->status == EXPECTING_PROPERTY) && (knownObjectElement(elementName))) return;
    if (f->status == EXPECTING_OBJECT) {
      char* url = NULL;
      RDF_Resource obj;
      uint16 count = 0;      
      url = getHref(attlist);
      if (url == NULL) url = getAttributeValue(attlist, "id"); 
      if (url) url = possiblyMakeAbsolute(f, url);
      obj =  ResourceFromElementName(f, url);
      if (url) freeMem(url);
      addToResourceList(f, obj);
      addElementProps (attlist, elementName, f, obj) ;
      if (!tagEquals(f, elementName, "RDF:Description")) {
        if (containerTagp(f, elementName)) {
          setContainerp(obj, 1);
        } else {
          RDF_Resource eln = ResourceFromElementName(f, elementName);
          addSlotValue(f, obj, gCoreVocab->RDF_instanceOf, eln, RDF_RESOURCE_TYPE, 1);
        }
      }
      if (f->depth > 1) {
        addSlotValue(f, f->stack[f->depth-2], f->stack[f->depth-1], obj, 
                     RDF_RESOURCE_TYPE, 1);
      }
      if (!emptyElementp) {
        f->stack[f->depth++] = obj;
        f->status = EXPECTING_PROPERTY;
      }
    } else if (f->status == EXPECTING_PROPERTY) {
      char* url;
      RDF_Resource obj;
      uint16 count = 0;
      url = getHref(attlist) ;      
      if (url) {
        RDF_Resource eln = ResourceFromElementName(f, elementName);
        url = possiblyMakeAbsolute(f, url);
        obj =  ResourceFromElementName(f, url);
        freeMem(url);
        addElementProps (attlist, elementName, f, obj) ;
        addToResourceList(f, obj);
        addSlotValue(f, f->stack[f->depth-1], eln,obj,RDF_RESOURCE_TYPE, 1);

      }
      if (!emptyElementp) {
        f->stack[f->depth++] = ResourceFromElementName(f, elementName);
        f->status = EXPECTING_OBJECT;
      }
    }
  }
}	



void
tokenizeElement (char* attr, char** attlist, char** elementName)
{
  size_t n = 1;
  size_t s = strlen(attr); 
  char c ;
  size_t m = 0;
  size_t atc = 0;
  PRBool emptyTagp =  (attr[s-2] == '/');
  PRBool inAttrNamep = 1;
  c = attr[n++]; 
  while (wsc(c)) {
    c = attr[n++];
  }
  *elementName = &attr[n-1];
  while (n < s) {
    if (wsc(c)) break;
    c = attr[n++];
  }
  attr[n-1] = '\0';
  while (atc < 2*MAX_ATTRIBUTES+1) {*(attlist + atc++) = NULL;}
  atc = 0;
  s = (emptyTagp ? s-2 : s-1);
  while (n < s) {
    PRBool attributeOpenStringSeenp = 0;
    m = 0;
    c = attr[n++];
    while ((n <= s) && (atc < 2*MAX_ATTRIBUTES)) {
      if (inAttrNamep && (m > 0) && (wsc(c) || (c == '='))) {
	attr[n-1] = '\0';
	*(attlist + atc++) = &attr[n-m-1];
	break;
      }
      if  (!inAttrNamep && attributeOpenStringSeenp && (c == '"')) {
	attr[n-1] = '\0';
	*(attlist + atc++) = &attr[n-m-1];
	break;
      }
      if (inAttrNamep) {
	if ((m > 0) || (!wsc(c))) m++;
      } else {
	if (c == '"') {
	  attributeOpenStringSeenp = 1;
	} else {
	  if ((m > 0) || (!(wsc(c)))) m++;
	}
      }
      c = attr[n++];
    }
    inAttrNamep = (inAttrNamep ? 0 : 1);
  }
}



void
outputRDFTreeInt (RDF rdf, PRFileDesc *fp, RDF_Resource node, uint32 depth, PRBool localOnly) 
{
  RDF_Cursor c = RDF_GetSources(rdf, node, gCoreVocab->RDF_parent, RDF_RESOURCE_TYPE,  true);
  RDF_Resource next;
  char* buf = getMem(1024);
  char* space = getMem((4*depth)+1);
  char* url   = resourceID(node);
  char* name  = RDF_GetResourceName(rdf, node);
  char* hrefid;

  if ((buf == NULL) || (space == NULL)) return;
  if (depth > 0) memset(space, ' ', depth);
  
  if (!strchr(url, ':') || (depth == 0)) {
	  hrefid = "id";
  } else {
	  hrefid = "rdf:href";
  }

  if (depth == 0) url="root";

  if (containerp(node)) {
    if (depth > 0) {
      sprintf(buf, "%s<child>\n", space);  
	  PR_Write(fp, buf, strlen(buf)); 
    }
    sprintf(buf, "%s<Topic %s=\"%s\"\n%s       name=\"%s\">\n", space, hrefid, url, space,   name);  
	PR_Write(fp, buf, strlen(buf));
    
    while ((next = RDF_NextValue(c)) != NULL) {
    
      /* if exporting EVERYTHING, need to skip over certain things */
      if ((localOnly == PR_FALSE) || ((!startsWith("ftp:",url)) && (!startsWith("file:",url))
	    && (!startsWith("IMAP:", url)) && (!startsWith("nes:", url))
	    && (!startsWith("mail:", url)) && (!startsWith("cache:", url))
	    && (!startsWith("ldap:", url)) &&
	    (!urlEquals(resourceID(node), resourceID(gNavCenter->RDF_LocalFiles))) &&
	    (!urlEquals(resourceID(node), resourceID(gNavCenter->RDF_History)))))
	{
		outputRDFTreeInt(rdf, fp, next, depth+1, localOnly);
	}
    }
    sprintf(buf, "%s</Topic>\n", space);
	PR_Write(fp, buf, strlen(buf));
    if (depth > 0) {
      sprintf(buf, "%s</child>\n", space);
	  PR_Write(fp, buf, strlen(buf));
    } 
  } else {
    sprintf(buf, "%s<child %s=\"%s\"\n%s       name=\"%s\"/>\n", space, hrefid, url, space,   name); 
	PR_Write(fp, buf, strlen(buf));
  }
  RDF_DisposeCursor(c);
  freeMem(buf);
  freeMem(space);
}



void
outputRDFTree (RDF rdf, PRFileDesc *fp, RDF_Resource node)
{
  ht_fprintf(fp, "<RDF:RDF>\n\n"); 
  outputRDFTreeInt(rdf, fp, node, 0, (node==gNavCenter->RDF_Top) ? PR_TRUE:PR_FALSE);
  ht_fprintf(fp, "\n</RDF:RDF>\n");
}
