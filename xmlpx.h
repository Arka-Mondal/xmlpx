/*
 * Copyright (c) 2024, Arka Mondal. All rights reserved.
 * Use of this source code is governed by a BSD-style license that
 * can be found in the LICENSE file.
*/

#ifndef _XMLPX_H_
#define _XMLPX_H_

#include <sys/types.h>
#include "lib/vector.h"

enum _xml_tag_type {
  ERR = -1,
  XML = 0,
  INFO,
  INLINE,
  USER_DEFINED,
};

struct _xml_node {
  char *tag;
  char *text;
  struct _xml_node *parent;
  vector_t attributes;
  vector_t children;
};

struct _xml_attribute {
  char *key;
  char *value;
};

struct _xmlpx_xmldoc {
  struct _xml_node *root;
  char *version;
  char *encoding;
};

typedef struct _xmlpx_xmldoc xmldoc_t;
typedef struct _xml_node xmlnode_t;
typedef struct _xml_attribute xmlattr_t;
typedef enum _xml_tag_type xmltagtype_t;

int xmlpx_load(xmldoc_t *restrict xml_doc, const char *restrict path);
int xmlpx_destroy(xmldoc_t *xml_doc);

xmlnode_t * xmlpx_node_new(xmlnode_t *parent);
void xmlpx_node_destroy(xmlnode_t *node);
int xmlpx_parse(xmldoc_t *restrict xml_doc, const char *restrict buf);
xmltagtype_t xmlpx_parse_tag_attrs(xmlnode_t *restrict curr, char *restrict buf, size_t len);

char * xmlpx_get_attr_val(xmlnode_t *node, const char *restrict attr);
void xmlpx_attr_destroy(xmlattr_t *attr);
xmlnode_t * xmlpx_get_child(xmlnode_t *parent, size_t index);

#endif
