/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/webui/brave_rewards_donate_ui.h"

#include "chrome/browser/android/tab_android.h"
#include "chrome/browser/sessions/session_tab_helper.h"
#include "chrome/browser/ui/android/tab_model/tab_model.h"
#include "chrome/browser/ui/android/tab_model/tab_model_list.h"
#include "brave/vendor/bat-native-ledger/include/bat/ledger/publisher_info.h"

#include "brave/components/brave_rewards/browser/rewards_service.h"
#include "brave/components/brave_rewards/browser/rewards_service_factory.h"
#include "brave/components/brave_rewards/browser/rewards_service_observer.h"
#include "brave/components/brave_rewards/browser/wallet_properties.h"
#include "components/brave_rewards/donate/resources/grit/brave_rewards_donate_generated_map.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/webui_url_constants.h"
#include "components/grit/components_resources.h"
#include "components/grit/components_scaled_resources.h"
#include "components/strings/grit/components_chromium_strings.h"
#include "components/strings/grit/components_strings.h"

#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/browser/web_ui.h"
#include "content/public/browser/web_ui_data_source.h"
#include "content/public/browser/web_ui_message_handler.h"

using content::WebUIMessageHandler;

namespace {


// The handler for Javascript messages for the chrome://rewards-panel page.
class RewardsDOMHandler : public WebUIMessageHandler,
                          public brave_rewards::RewardsServiceObserver {
 public:
  RewardsDOMHandler() {};
  ~RewardsDOMHandler() override;

  void Init();

  // WebUIMessageHandler implementation.
  void RegisterMessages() override;

private:
  brave_rewards::RewardsService* rewards_service_;

  void GetPublisherData(const base::ListValue* args);
  void GetCurrentActiveTabInfo(const base::ListValue* args);
  void GetPublisherDonateData(const base::ListValue* args);
  void OnDonate(const base::ListValue* args);
  void GetWalletProperties(const base::ListValue* args);
	void OnWalletProperties(brave_rewards::RewardsService* rewards_service,
	                         int error_code,
	                         std::unique_ptr<brave_rewards::WalletProperties>  wallet_properties) override;
  void GetRecurringDonations(const base::ListValue* args);

  void OnRecurringDonationUpdated(brave_rewards::RewardsService* rewards_service,
                                  brave_rewards::ContentSiteList) override;
  void OnPublisherBanner(brave_rewards::RewardsService* rewards_service,
                         const brave_rewards::PublisherBanner banner) override;
  void OnDialogClosed(const base::ListValue* args);
  void OnGetPublisherActivityFromUrl(
     brave_rewards::RewardsService* rewards_service,
     int error_code,
     ledger::PublisherInfo* info,
     uint64_t tabId);
  DISALLOW_COPY_AND_ASSIGN(RewardsDOMHandler);
};


RewardsDOMHandler::~RewardsDOMHandler() {
  if (rewards_service_)
    rewards_service_->RemoveObserver(this);
}

void RewardsDOMHandler::RegisterMessages() {
	web_ui()->RegisterMessageCallback("brave_rewards_donate.getWalletProperties",
      base::BindRepeating(&RewardsDOMHandler::GetWalletProperties,
                          base::Unretained(this)));
  web_ui()->RegisterMessageCallback("brave_rewards_donate.getRecurringDonations",
      base::BindRepeating(&RewardsDOMHandler::GetRecurringDonations,
                          base::Unretained(this)));
  web_ui()->RegisterMessageCallback("brave_rewards_donate.getPublisherBanner",
      base::BindRepeating(&RewardsDOMHandler::GetPublisherDonateData,
                          base::Unretained(this)));
  web_ui()->RegisterMessageCallback("brave_rewards_donate.onDonate",
      base::BindRepeating(&RewardsDOMHandler::OnDonate,
                          base::Unretained(this)));
  web_ui()->RegisterMessageCallback("dialogClose", 
      base::BindRepeating(&RewardsDOMHandler::OnDialogClosed,
                          base::Unretained(this)));
  web_ui()->RegisterMessageCallback("brave_rewards_donate.getCurrentActiveTabInfo",
      base::BindRepeating(&RewardsDOMHandler::GetCurrentActiveTabInfo,
                          base::Unretained(this)));
  web_ui()->RegisterMessageCallback("brave_rewards_donate.getPublisherData",
      base::BindRepeating(&RewardsDOMHandler::GetPublisherData,
                          base::Unretained(this)));
}

void RewardsDOMHandler::GetPublisherData(const base::ListValue* args) {
  std::string tabIdStr;
  std::string url;
  args->GetString(0, &tabIdStr);
  args->GetString(1, &url);
  std::stringstream tempTabId(tabIdStr);
  SessionID::id_type tabId = -1;
  tempTabId >> tabId;

  if (rewards_service_) {
    rewards_service_->GetPublisherActivityFromUrl(tabId, url, "");
  }
}

void RewardsDOMHandler::OnGetPublisherActivityFromUrl(
      brave_rewards::RewardsService* rewards_service,
      int error_code,
      ledger::PublisherInfo* info,
      uint64_t tabId) {
  if (!info || !web_ui()->CanCallJavascript()) {
    return;
  }

  base::DictionaryValue data;
  data.SetString("tabId", std::to_string(tabId).c_str());
  auto publisher = std::make_unique<base::DictionaryValue>();
  publisher->SetString("publisher_key", info->id);
  data.SetDictionary("publisher", std::move(publisher));

  web_ui()->CallJavascriptFunctionUnsafe("brave_rewards_donate.publisherData", data);
}

void RewardsDOMHandler::GetCurrentActiveTabInfo(const base::ListValue* args) {
  if (!web_ui()->CanCallJavascript()) {
    return;
  }

  for (TabModelList::const_iterator iter = TabModelList::begin(); iter < TabModelList::end(); iter++) {
    TabModel* model = *iter;
    if (!model) {
      continue;
    }
    content::WebContents* web_contents = model->GetActiveWebContents();
    // Check are we on incognito TabModel
    if (web_contents && web_contents->GetBrowserContext()->IsOffTheRecord()) {
      continue;
    }
    TabAndroid* tabAndroid = model->GetTabAt(model->GetActiveIndex());
    base::DictionaryValue currentTabInfo;
    currentTabInfo.SetString("id", std::to_string(SessionTabHelper::IdForTab(web_contents).id()).c_str());
    currentTabInfo.SetString("url", tabAndroid->GetURL().spec());

    web_ui()->CallJavascriptFunctionUnsafe("brave_rewards_donate.currentTabInfo", currentTabInfo);
  }
}

void RewardsDOMHandler::OnDialogClosed(const base::ListValue* args) {
  content::WebContentsDelegate* delegate = web_ui()->GetWebContents()->GetDelegate();
  if (delegate) {
    delegate->CloseContents(web_ui()->GetWebContents());
  }
}

void RewardsDOMHandler::GetPublisherDonateData(const base::ListValue* args) {
  std::string publisher_key;
  args->GetString(0, &publisher_key);
  rewards_service_->GetPublisherBanner(publisher_key);
}

void RewardsDOMHandler::OnDonate(const base::ListValue* args) {
  if (!rewards_service_ || !args)
    return;

  std::string publisher_key;
  int amount;
  bool recurring;
  args->GetString(0, &publisher_key);
  args->GetInteger(1, &amount);
  args->GetBoolean(2, &recurring);

  if (publisher_key.empty() || amount < 1) {
    // TODO add error
    return;
  }

  rewards_service_->OnDonate(publisher_key, amount, recurring);
}

void RewardsDOMHandler::OnPublisherBanner(brave_rewards::RewardsService* rewards_service,
                                                const brave_rewards::PublisherBanner banner) {
  if (!web_ui()->CanCallJavascript()) {
     return;
  }

  base::DictionaryValue result;
  result.SetString("publisherKey", banner.publisher_key);
  result.SetString("title", banner.title);
  result.SetString("name", banner.name);
  result.SetString("description", banner.description);
  result.SetString("background", banner.background);
  result.SetString("logo", banner.logo);

  auto amounts = std::make_unique<base::ListValue>();
  for (int const& value : banner.amounts) {
    amounts->AppendInteger(value);
  }
  result.SetList("amounts", std::move(amounts));

  auto social = std::make_unique<base::DictionaryValue>();
  for (auto const& item : banner.social) {
    social->SetString(item.first, item.second);
  }
  result.SetDictionary("social", std::move(social));

  web_ui()->CallJavascriptFunctionUnsafe("brave_rewards_donate.publisherBanner", result);
}

void RewardsDOMHandler::GetWalletProperties(const base::ListValue* args) {
  if (!rewards_service_)
    return;

  rewards_service_->FetchWalletProperties();
}

void RewardsDOMHandler::OnWalletProperties(
    brave_rewards::RewardsService* rewards_service,
    int error_code,
    std::unique_ptr<brave_rewards::WalletProperties> wallet_properties) {
  if (!wallet_properties || !web_ui()->CanCallJavascript()) {
    return;
  }

  base::DictionaryValue result;
  result.SetInteger("status", error_code);
  auto walletInfo = std::make_unique<base::DictionaryValue>();

  if (error_code == 0 && wallet_properties) {
    walletInfo->SetDouble("balance", wallet_properties->balance);
    walletInfo->SetString("probi", wallet_properties->probi);

    auto rates = std::make_unique<base::DictionaryValue>();
    for (auto const& rate : wallet_properties->rates) {
      rates->SetDouble(rate.first, rate.second);
    }
    walletInfo->SetDictionary("rates", std::move(rates));

    auto choices = std::make_unique<base::ListValue>();
    for (double const& choice : wallet_properties->parameters_choices) {
      choices->AppendDouble(choice);
    }
    walletInfo->SetList("choices", std::move(choices));

    auto range = std::make_unique<base::ListValue>();
    for (double const& value : wallet_properties->parameters_range) {
      range->AppendDouble(value);
    }
    walletInfo->SetList("range", std::move(range));

    auto grants = std::make_unique<base::ListValue>();
    for (auto const& item : wallet_properties->grants) {
      auto grant = std::make_unique<base::DictionaryValue>();
      grant->SetString("probi", item.probi);
      grant->SetInteger("expiryTime", item.expiryTime);
      grants->Append(std::move(grant));
    }
    walletInfo->SetList("grants", std::move(grants));
  }

  result.SetDictionary("wallet", std::move(walletInfo));

  web_ui()->CallJavascriptFunctionUnsafe("brave_rewards_donate.walletProperties", result);
}

void RewardsDOMHandler::GetRecurringDonations(const base::ListValue *args) {
  if (rewards_service_) {
    rewards_service_->UpdateRecurringDonationsList();
  }
}

void RewardsDOMHandler::OnRecurringDonationUpdated(brave_rewards::RewardsService* rewards_service,
                                                   const brave_rewards::ContentSiteList list) {
  if (!web_ui()->CanCallJavascript()) {
    return;
  }
  auto publishers = std::make_unique<base::ListValue>();
  for (auto const& item : list) {
    publishers->AppendString(item.id);
  }

  web_ui()->CallJavascriptFunctionUnsafe("brave_rewards_donate.recurringDonations", *publishers);
}

void RewardsDOMHandler::Init() {
  Profile* profile = Profile::FromWebUI(web_ui());
  rewards_service_ = brave_rewards::RewardsServiceFactory::GetForProfile(profile);
  if (rewards_service_)
    rewards_service_->AddObserver(this);
 }
}  // namespace

///////////////////////////////////////////////////////////////////////////////
//
// BraveRewardsDonateUI
//
///////////////////////////////////////////////////////////////////////////////

BraveRewardsDonateUI::BraveRewardsDonateUI(content::WebUI* web_ui, const std::string& name)
    : BasicUI(web_ui, name, kBraveRewardsDonateGenerated,
        kBraveRewardsDonateGeneratedSize, IDR_BRAVE_REWARDS_DONATE_HTML) {

  auto handler_owner = std::make_unique<RewardsDOMHandler>();
  RewardsDOMHandler * handler = handler_owner.get();
  web_ui->AddMessageHandler(std::move(handler_owner));
  handler->Init();
}

BraveRewardsDonateUI::~BraveRewardsDonateUI() {
}