/*
 * Copyright (c) 2024, Arka Mondal. All rights reserved.
 * Use of this source code is governed by a BSD-style license that
 * can be found in the LICENSE file.
*/

#ifndef _XMLPX_H_
#define _XMLPX_H_

#include <sys/types.h>

struct _xml_node {
  char *tag;
  char *text;
  struct _xml_node *parent;
};

struct _xmlpx_xmldoc {
  struct _xml_node *root;
};

typedef struct _xmlpx_xmldoc xmldoc_t;
typedef struct _xml_node xmlnode_t;

int xmlpx_load(xmldoc_t *restrict xml_doc, const char *restrict path);
int xmlpx_destroy(xmldoc_t *xml_doc);

xmlnode_t * xmlpx_node_new(xmlnode_t *parent);
void xmlpx_node_destroy(xmlnode_t *node);
int xmlpx_parse(xmldoc_t *restrict xml_doc, const char *restrict buf);

#endif
