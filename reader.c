/*
 * Copyright (c) 2024, Arka Mondal. All rights reserved.
 * Use of this source code is governed by a BSD-style license that
 * can be found in the LICENSE file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "xmlpx.h"

int xmlpx_load(xmldoc_t *restrict xml_doc, const char *restrict path)
{
  char *buf;
  int rv;
  FILE *fp;
  struct stat sbuf;

  if (path == NULL || xml_doc == NULL)
    return -1;

  fp = fopen(path, "r");
  if (fp == NULL)
    return -1;

  if (fstat(fileno(fp), &sbuf) == -1)
  {
    fclose(fp);
    return -1;
  }

  buf = malloc(sizeof(char) * (sbuf.st_size + 1));
  if (buf == NULL)
  {
    fclose(fp);
    return -1;
  }

  if (fread(buf, sbuf.st_size, 1, fp) == 0)
  {
    fclose(fp);
    free(buf);
    return -1;
  }

  fclose(fp);

  buf[sbuf.st_size] = '\0';

  rv = xmlpx_parse(xml_doc, buf);
  free(buf);

  return rv;
}

int xmlpx_destroy(xmldoc_t *xml_doc)
{
  if (xml_doc == NULL || xml_doc->root == NULL)
    return -1;

  // free(xml_doc->buf);
  // xml_doc->buf = NULL;
  // xml_doc->size = 0;

  return 0;
}
