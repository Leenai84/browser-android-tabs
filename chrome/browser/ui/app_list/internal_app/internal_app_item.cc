// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/app_list/internal_app/internal_app_item.h"

#include "ash/public/cpp/app_list/app_list_config.h"
#include "base/metrics/histogram_macros.h"
#include "chrome/browser/ui/app_list/app_context_menu.h"
#include "chrome/browser/ui/app_list/app_list_model_updater.h"
#include "chrome/browser/ui/app_list/internal_app/internal_app_metadata.h"
#include "ui/base/l10n/l10n_util.h"

namespace {

void RecordActiveHistogramInternal(app_list::InternalAppName name) {
  UMA_HISTOGRAM_ENUMERATION("Apps.AppListInternalApp.Activate", name);
}

}  // namespace

// static
const char InternalAppItem::kItemType[] = "InternalAppItem";

// static
void InternalAppItem::RecordActiveHistogram(const std::string& app_id) {
  RecordActiveHistogramInternal(app_list::GetInternalAppNameByAppId(app_id));
}

InternalAppItem::InternalAppItem(
    Profile* profile,
    AppListModelUpdater* model_updater,
    const app_list::AppListSyncableService::SyncItem* sync_item,
    const app_list::InternalApp& internal_app)
    : ChromeAppListItem(profile, internal_app.app_id) {
  SetIcon(app_list::GetIconForResourceId(
      internal_app.icon_resource_id,
      app_list::AppListConfig::instance().grid_icon_dimension()));
  SetName(l10n_util::GetStringUTF8(internal_app.name_string_resource_id));
  if (sync_item && sync_item->item_ordinal.IsValid())
    UpdateFromSync(sync_item);
  else
    SetDefaultPositionIfApplicable(model_updater);
}

InternalAppItem::~InternalAppItem() = default;

const char* InternalAppItem::GetItemType() const {
  return InternalAppItem::kItemType;
}

void InternalAppItem::Activate(int event_flags) {
  RecordActiveHistogram(id());
  app_list::OpenInternalApp(id(), profile(), event_flags);
}

void InternalAppItem::GetContextMenuModel(GetMenuModelCallback callback) {
  if (!context_menu_) {
    context_menu_ = std::make_unique<app_list::AppContextMenu>(
        nullptr, profile(), id(), GetController());
  }
  context_menu_->GetMenuModel(std::move(callback));
}

app_list::AppContextMenu* InternalAppItem::GetAppContextMenu() {
  return context_menu_.get();
}
