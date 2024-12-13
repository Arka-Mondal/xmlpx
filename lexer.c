/*
 * Copyright (c) 2024, Arka Mondal. All rights reserved.
 * Use of this source code is governed by a BSD-style license that
 * can be found in the LICENSE file.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "xmlpx.h"

xmlnode_t * xmlpx_node_new(xmlnode_t *parent)
{
  xmlnode_t *node;

  node = malloc(sizeof(xmlnode_t));
  if (node == NULL)
    return NULL;

  node->tag = NULL;
  node->text = NULL;
  node->parent = parent;

  return node;
}

void xmlpx_node_destroy(xmlnode_t *node)
{
  if (node == NULL)
    return;

  if (node->tag)
    free(node->tag);
  if (node->text)
    free(node->text);

  free(node);
}

int xmlpx_parse(xmldoc_t *restrict xml_doc, const char *restrict buf)
{
  static char lex[1024];
  int lex_index, i;
  xmlnode_t *curr, *temp;

  i = 0;
  lex_index = 0;
  curr = NULL;

  memset(xml_doc, 0, sizeof(xmldoc_t));

  while (buf[i] != '\0')
  {
    if (buf[i] == '<')
    {
      lex[lex_index] = '\0';
      if (lex_index > 0)
      {
        if (curr == NULL)
        {
          fprintf(stderr, "Enclosing Tag not found for: <?>%s</?>\n", lex);
          return -1;
        }

        curr->text = strdup(lex);
        if (curr->text == NULL)
          return -1;

        // I have consumed the lex-buf
        lex_index = 0;
      }

      if (buf[i + 1] == '/')
      {
        i += 2;
        while (buf[i] != '>')
          lex[lex_index++] = buf[i++];

        lex[lex_index] = '\0';

        if (curr == NULL)
        {
          fprintf(stderr, "Opening Tag not found for: %s\n", lex);
          return -1;
        }

        if (strcmp(curr->tag, lex) != 0)
        {
          fprintf(stderr, "Tag mismatch: (<%s></%s>)\n", curr->tag, lex);
          return -1;
        }

        // I have consumed the lex-buf
        lex_index = 0;

        curr = curr->parent;
      }
      else
      {
        if (curr == NULL && xml_doc->root != NULL)
        {
          fprintf(stderr, "Tag hierarchy mismatch\n");
          return -1;
        }

        temp = xmlpx_node_new(curr);
        if (temp == NULL)
          return -1;

        if (curr == NULL)
          xml_doc->root = temp;
        curr = temp;

        i++; // skip the '<'
        while (buf[i] != '>')
          lex[lex_index++] = buf[i++];

        lex[lex_index] ='\0';
        curr->tag = strdup(lex);
        if (curr->tag == NULL)
          return -1;

        // I have consumed the lex-buf
        lex_index = 0;
      }
    }
    else
    {
      lex[lex_index++] = buf[i];
    }
    i++;
  }

  return 0;
}
