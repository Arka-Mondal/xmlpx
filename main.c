/*
 * Copyright (c) 2024, Arka Mondal. All rights reserved.
 * Use of this source code is governed by a BSD-style license that
 * can be found in the LICENSE file.
*/

#include <stdio.h>
#include "xmlpx.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
  xmldoc_t doc;
  xmlnode_t *tnode;

  if (xmlpx_load(&doc, "test.xml") == 0)
  {
    printf("Version: %s, Encoding: %s\n", doc.version, doc.encoding);
    tnode = xmlpx_get_child(xmlpx_get_child(doc.root, 0), 1);
    if (tnode)
    {
      printf("%s:%s\n", tnode->tag, tnode->text);
      printf("%s=%s\n", "name", xmlpx_get_attr_val(tnode, "name"));
    }
    else
      puts("nil");

  }

  xmlpx_destroy(&doc);

  return 0;
}
