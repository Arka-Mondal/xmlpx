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

  if (xmlpx_load(&doc, "test.xml") == 0)
  {
    printf("%s: %s\n", doc.root->tag, doc.root->text);
    xmlpx_destroy(&doc);
  }

  return 0;
}
