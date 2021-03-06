// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_CONTENT_PUBLIC_CPP_TEST_FAKE_NAVIGABLE_CONTENTS_FACTORY_H_
#define SERVICES_CONTENT_PUBLIC_CPP_TEST_FAKE_NAVIGABLE_CONTENTS_FACTORY_H_

#include "base/macros.h"
#include "mojo/public/cpp/bindings/binding_set.h"
#include "services/content/public/mojom/navigable_contents_factory.mojom.h"

namespace content {

class FakeNavigableContents;

// Usable by unit tests which drive Content Service client code. Allows tests
// to fake a simple Content Service implementation which drives and customizes
// basic navigation lifecycle events.
class FakeNavigableContentsFactory : public mojom::NavigableContentsFactory {
 public:
  FakeNavigableContentsFactory();
  ~FakeNavigableContentsFactory() override;

  // Bind a new factory request. A single FakeNavigableContentsFactory supports
  // binding any number of requests simultaneously.
  void BindRequest(mojom::NavigableContentsFactoryRequest request);

  // Waits for the next |CreateContents()| request on the factory and fulfills
  // it by binding to |*contents|.
  void WaitForAndBindNextContentsRequest(FakeNavigableContents* contents);

 private:
  // mojom::NavigableContentsFactory:
  void CreateContents(mojom::NavigableContentsParamsPtr params,
                      mojom::NavigableContentsRequest request,
                      mojom::NavigableContentsClientPtr client) override;

  mojo::BindingSet<mojom::NavigableContentsFactory> bindings_;

  using CreateContentsCallback =
      base::OnceCallback<void(mojom::NavigableContentsRequest,
                              mojom::NavigableContentsClientPtr)>;
  CreateContentsCallback next_create_contents_callback_;

  DISALLOW_COPY_AND_ASSIGN(FakeNavigableContentsFactory);
};

}  // namespace content

#endif  // SERVICES_CONTENT_PUBLIC_CPP_TEST_FAKE_NAVIGABLE_CONTENTS_FACTORY_H_
