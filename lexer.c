/*
 * Copyright (c) 2024, Arka Mondal. All rights reserved.
 * Use of this source code is governed by a BSD-style license that
 * can be found in the LICENSE file.
*/

#include <ctype.h>
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
  if (!clibds_vec_init(&node->attributes, xmlattr_t))
  {
    free(node);
    return NULL;
  }

  if (!clibds_vec_init(&node->children, xmlnode_t *))
  {
    clibds_vec_delete(&node->attributes);
    free(node);
    return NULL;
  }

  if (parent == NULL)
    return node;


  if (!clibds_vec_push(&parent->children, &node))
  {
    clibds_vec_delete(&node->attributes);
    clibds_vec_delete(&node->children);
    free(node);
    return NULL;
  }

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

  clibds_vec_foreach(&node->attributes, itr)
    xmlpx_attr_destroy(&clibds_vec_format_data(itr, xmlattr_t));

  clibds_vec_delete(&node->attributes);

  clibds_vec_foreach(&node->children, itr)
    xmlpx_node_destroy(clibds_vec_format_data(itr, xmlnode_t *));

  clibds_vec_delete(&node->children);

  free(node);
}

char * xmlpx_get_attr_val(xmlnode_t *node, const char *restrict attr)
{
  if (node == NULL || attr == NULL)
    return NULL;

  clibds_vec_foreach(&node->attributes, it)
  {
    if (strcmp(clibds_vec_format_data(it, xmlattr_t).key, attr) == 0)
      return clibds_vec_format_data(it, xmlattr_t).value;
  }

  return NULL;
}

void xmlpx_attr_destroy(xmlattr_t *attr)
{
  if (attr == NULL)
    return;

  if (attr->key)
    free(attr->key);
  if (attr->value)
    free(attr->value);
}

int xmlpx_parse(xmldoc_t *restrict xml_doc, const char *restrict buf)
{
  static char lex[1024];
  size_t lex_index, i;
  xmlnode_t *curr, *temp;
  xmltagtype_t ttype;

  i = 0;
  lex_index = 0;

  curr = xmlpx_node_new(NULL);
  if (curr == NULL)
    return -1;

  xml_doc->root = curr;
  xml_doc->encoding = NULL;
  xml_doc->version = NULL;


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

        lex_index--;
        while (lex_index > 0 && isspace(lex[lex_index]))
          lex_index--;
        if (lex_index > 0)
          lex_index++;


        curr->text = strndup(lex, lex_index);
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

        if (curr->tag == NULL || strcmp(curr->tag, lex) != 0)
        {
          fprintf(stderr, "Tag mismatch: (<%s></%s>)\n", curr->tag, lex);
          return -1;
        }

        // I have consumed the lex-buf
        lex_index = 0;

        curr = curr->parent;
      }
      else if (buf[i + 1] == '!')
      {
        if (strncmp(&buf[i], "<!--", 3))
        {
          fprintf(stderr, "Invalid comment format: at(%zu)\n", i);
          return -1;
        }

        i += 4;
        while (strncmp(&buf[i], "-->", 3))
        {
          if (buf[i] == '\0' || strncmp(&buf[i], "<!--", 3) == 0)
          {
            fprintf(stderr, "Invalid comment format: at(%zu)\n", i);
            return -1;
          }
          i++;
        }

        i += 3;
      }
      else
      {
        temp = xmlpx_node_new(curr);
        if (temp == NULL)
          return -1;

        curr = temp;

        i++; // skip the '<'
        while (buf[i] != '>')
          lex[lex_index++] = buf[i++];

        lex[lex_index] ='\0';

        ttype = xmlpx_parse_tag_attrs(curr, lex, lex_index);
        if (ttype == ERR)
          return -1;
        else if (ttype == XML || ttype == INFO)
        {
          if (ttype == XML)
          {
            clibds_vec_foreach(&curr->attributes, it)
            {
              if (strcmp(clibds_vec_format_data(it, xmlattr_t).key, "encoding") == 0)
              {
                xml_doc->encoding = clibds_vec_format_data(it, xmlattr_t).value;
                clibds_vec_format_data(it, xmlattr_t).value = NULL;
              }
              else if (strcmp(clibds_vec_format_data(it, xmlattr_t).key, "version") == 0)
              {
                xml_doc->version = clibds_vec_format_data(it, xmlattr_t).value;
                clibds_vec_format_data(it, xmlattr_t).value = NULL;
              }
            }
          }
          clibds_vec_pop(&curr->parent->children);
          temp = curr;
          curr = curr->parent;
          xmlpx_node_destroy(temp);
        }
        else if (ttype == INLINE)
        {
          temp = curr;
          curr = curr->parent;
        }

        lex_index = 0;
      }
      while (isspace(buf[i + 1]))
        i++;
    }
    else
    {
      lex[lex_index++] = buf[i];
    }
    i++;
  }

  if (lex_index > 0)
  {
    fprintf(stderr, "Text out-of tag: (%.*s)\n", (int) lex_index, lex);
    return -1;
  }

  return 0;
}

xmltagtype_t xmlpx_parse_tag_attrs(xmlnode_t *restrict curr, char *restrict buf, size_t len)
{
  char *temp_key, *temp_value;
  size_t s, e;
  xmlattr_t temp_attr;
  xmltagtype_t ttype;

  if (curr == NULL || buf == NULL)
    return ERR;

  s = 0;
  e = 0;

  if (buf[s] == '?')
  {
    if (buf[s] != buf[len - 1])
    {
      fprintf(stderr, "Invalid tag: %s\n", buf);
      return ERR;
    }

    s++;
    while (s < len && isspace(buf[s]))
      s++;

    e = s;
    while (e < len && !isspace(buf[e]))
      e++;
  }
  else
  {
    // get the tag first
    while (s < len && isspace(buf[s]))
      s++;

    e = s;
    while (e < len && !isspace(buf[e]))
      e++;

    if (( ispunct(buf[s])
      || isdigit(buf[s]) )
      || ((e - s >= 3)
        && (tolower(buf[s]) == 'x' && tolower(buf[s + 1]) == 'm' &&  tolower(buf[s + 2]) == 'l')) )
    {
      fprintf(stderr, "Invalid tag: %s\n", buf);
      return ERR;
    }

  }

  if (strncmp(&buf[s], "xml", e - s) == 0)
  {
    ttype = XML;
    buf[len - 1] = '\0';
    len--;
  }
  else
  {
    if (buf[len - 1] == '?')
    {
      ttype = INFO;
      buf[len - 1] = '\0';
      len--;
    }
    else if (buf[len - 1] == '/')
    {
      ttype = INLINE;
      buf[len - 1] = '\0';
      len--;
    }
    else
      ttype = USER_DEFINED;
  }

  curr->tag = strndup(buf + s, e - s);
  if (curr->tag == NULL)
    return ERR;

  // get the attributes
  while (1)
  {
    s = e;
    while (s < len && isspace(buf[s]))
      s++;

    // out-of-buffer
    if (s >= len)
      break;

    e = s;
    while (e < len && (buf[e] != '=' && !isspace(buf[e])))
      e++;

    if (e >= len || isspace(buf[e]))
    {
      fprintf(stderr, "bad key:value format - (%zu:%zu) %c\n", s, e - 1, buf[s]);
      return ERR;
    }

    temp_key = strndup(buf + s, e - s);
    if (temp_key == NULL)
      return ERR;

    s = e;
    while (s < len && (buf[s] != '"' && buf[s] != '\''))
      s++;

    s++;
    if (s >= len)
      break;

    e = s;
    while (e < len && buf[e] != buf[s - 1])
      e++;

    if (e >= len)
    {
      fprintf(stderr, "bad value format - (%zu:%zu) %c\n", s, e - 1, buf[s]);
      return ERR;
    }


    temp_value = strndup(buf + s, e - s);
    if (temp_value == NULL)
      return ERR;
    e++;

    temp_attr.key = temp_key;
    temp_attr.value = temp_value;
    if (!clibds_vec_push(&curr->attributes, &temp_attr))
      return ERR;
  }


  return ttype;
}

xmlnode_t * xmlpx_get_child(xmlnode_t *parent, size_t index)
{
  void *temp;
  if (parent == NULL)
    return NULL;

  temp = clibds_vec_getatindex(&parent->children, index);

  return (temp) ? *(xmlnode_t **) temp : NULL;
}
