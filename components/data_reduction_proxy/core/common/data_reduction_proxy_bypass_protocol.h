// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_DATA_REDUCTION_PROXY_CORE_COMMON_DATA_REDUCTION_PROXY_BYPASS_PROTOCOL_H_
#define COMPONENTS_DATA_REDUCTION_PROXY_CORE_COMMON_DATA_REDUCTION_PROXY_BYPASS_PROTOCOL_H_

#include "base/macros.h"
#include "base/optional.h"
#include "base/threading/thread_checker.h"
#include "components/data_reduction_proxy/core/common/data_reduction_proxy_headers.h"
#include "net/base/net_errors.h"

namespace data_reduction_proxy {

struct DataReductionProxyTypeInfo;

// Class responsible for determining when a response should or should not cause
// the data reduction proxy to be bypassed, and to what degree. Owned by the
// DataReductionProxyInterceptor.
class DataReductionProxyBypassProtocol {
 public:
  // TODO(http://crbug.com/721403): This interface only exists as an
  // intermediary step to avoid depending on data_reduction_proxy/core/browser.
  // The correct dependency is DataReductionProxyBypassStats.
  class Stats {
   public:
    virtual ~Stats();

    virtual void RecordDataReductionProxyBypassInfo(
        bool is_primary,
        bool bypass_all,
        const net::ProxyServer& proxy_server,
        DataReductionProxyBypassType bypass_type) = 0;

    virtual void DetectAndRecordMissingViaHeaderResponseCode(
        bool is_primary,
        const net::HttpResponseHeaders& headers) = 0;
  };

  // Enum values that can be reported for the
  // DataReductionProxy.ResponseProxyServerStatus histogram. These values must
  // be kept in sync with their counterparts in histograms.xml. Visible here for
  // testing purposes.
  enum ResponseProxyServerStatus {
    RESPONSE_PROXY_SERVER_STATUS_EMPTY = 0,
    RESPONSE_PROXY_SERVER_STATUS_DRP,
    RESPONSE_PROXY_SERVER_STATUS_NON_DRP_NO_VIA,
    RESPONSE_PROXY_SERVER_STATUS_NON_DRP_WITH_VIA,
    RESPONSE_PROXY_SERVER_STATUS_MAX
  };

  explicit DataReductionProxyBypassProtocol(Stats* stats);

  // Decides whether to restart the request, whether to bypass proxies when
  // doing so, and whether to mark any data reduction proxies as bad based on
  // the response.
  // Returns true if the request should be retried. Sets
  // |additional_load_flags_for_restart| with any load flags that should be
  // used when restarting the request (e.g., in response to "block-once" will
  // bypass cache and proxies). Fills |bad_proxies| with the list of proxies to
  // mark as bad.
  bool MaybeBypassProxyAndPrepareToRetry(
      const std::string& method,
      const std::vector<GURL>& url_chain,
      const net::HttpResponseHeaders* response_headers,
      const net::ProxyServer& proxy_server,
      net::Error net_error,
      const net::ProxyRetryInfoMap& proxy_retry_info,
      const base::Optional<DataReductionProxyTypeInfo>& proxy_type_info,
      DataReductionProxyBypassType* proxy_bypass_type,
      DataReductionProxyInfo* data_reduction_proxy_info,
      std::vector<net::ProxyServer>* bad_proxies,
      int* additional_load_flags_for_restart);

 private:
  // Decides whether to mark the data reduction proxy as temporarily bad and
  // put it on the proxy retry map. Returns true if the request should be
  // retried. Should be called only when the response had null response headers.
  bool HandleInvalidResponseHeadersCase(
      const std::vector<GURL>& url_chain,
      net::Error net_error,
      const base::Optional<DataReductionProxyTypeInfo>&
          data_reduction_proxy_type_info,
      DataReductionProxyInfo* data_reduction_proxy_info,
      DataReductionProxyBypassType* bypass_type);

  // Decides whether to mark the data reduction proxy as temporarily bad and
  // put it on the proxy retry map. Returns true if the request should be
  // retried. Should be called only when the response had non-null response
  // headers.
  bool HandleValidResponseHeadersCase(
      const std::vector<GURL>& url_chain,
      const net::HttpResponseHeaders* response_headers,
      const net::ProxyRetryInfoMap& proxy_retry_info,
      const DataReductionProxyTypeInfo& data_reduction_proxy_type_info,
      DataReductionProxyBypassType* proxy_bypass_type,
      DataReductionProxyInfo* data_reduction_proxy_info,
      DataReductionProxyBypassType* bypass_type);

  // Must outlive |this|.
  Stats* stats_;

  base::ThreadChecker thread_checker_;

  DISALLOW_COPY_AND_ASSIGN(DataReductionProxyBypassProtocol);
};

// Returns true if the proxy is on the retry map and the retry delay is not
// expired. If proxy is bypassed, retry_delay (if not NULL) returns the delay
// of proxy_server. If proxy is not bypassed, retry_delay is not assigned.
//
// TODO(http://crbug.com/721403): Move this somewhere better.
bool IsProxyBypassedAtTime(const net::ProxyRetryInfoMap& retry_map,
                           const net::ProxyServer& proxy_server,
                           base::TimeTicks t,
                           base::TimeDelta* retry_delay);

}  // namespace data_reduction_proxy

#endif  // COMPONENTS_DATA_REDUCTION_PROXY_CORE_COMMON_DATA_REDUCTION_PROXY_BYPASS_PROTOCOL_H_
