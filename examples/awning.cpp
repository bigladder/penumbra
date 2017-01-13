/* Copyright (c) 2017 Big Ladder Software and Chip Barnaby. All rights reserved.
* See the LICENSE file for additional terms and conditions. */

#include "penumbra/penumbra.h"
#include <iostream>

int main(void)
{
    std::vector<std::array<float,3>> model =
    {
      {{ 0.f, 0.f, 0.f}},
      {{ 1.f, 0.f, 0.f}},
      {{ 0.f, 0.f, 1.f}},
      {{ 0.f, 0.f, 1.f}},
      {{ 1.f, 0.f, 0.f}},
      {{ 1.f, 0.f, 1.f}},
      {{ 0.25f, 0.f, 0.5f}},
      {{ 0.75f, 0.f, 0.5f}},
      {{ 0.25f, -0.5f, 0.5f}},
      {{ 0.75f, 0.f, 0.5f}},
      {{ 0.25f, -0.5f, 0.5f}},
      {{ 0.75f, -0.5f, 0.5f}}
    };
    std::vector<std::array<float, 3>> surface =
    {
      { { 0.f, 0.f, 0.f } },
      { { 1.f, 0.f, 0.f } },
      { { 0.f, 0.f, 1.f } },
      { { 0.f, 0.f, 1.f } },
      { { 1.f, 0.f, 0.f } },
      { { 1.f, 0.f, 1.f } }
    };

    Pumbra::Context context(200);
    context.setModel(model);

    Pumbra::Sun sun(2.00f,0.4f);

    context.setScene(surface, sun.getView());
    context.showRendering();
    std::size_t pixels = context.countPixels();

    std::cout << pixels;

    return 0;
}
