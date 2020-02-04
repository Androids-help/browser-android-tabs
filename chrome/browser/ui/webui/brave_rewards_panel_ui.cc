/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/webui/brave_rewards_panel_ui.h"

#include "brave/components/brave_rewards/browser/rewards_service.h"
#include "brave/components/brave_rewards/browser/rewards_service_factory.h"
#include "brave/components/brave_rewards/browser/rewards_service_observer.h"

#include "chrome/browser/profiles/profile.h"
#include "chrome/common/webui_url_constants.h"
#include "components/grit/components_resources.h"
#include "components/grit/components_scaled_resources.h"
#include "components/strings/grit/components_chromium_strings.h"
#include "components/strings/grit/components_strings.h"

#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_ui.h"
#include "content/public/browser/web_ui_data_source.h"
#include "content/public/browser/web_ui_message_handler.h"

using content::WebContents;
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
   void GetCurrentWindowId(const base::ListValue* args);
   brave_rewards::RewardsService* rewards_service_;
   void GetBalanceReports(const base::ListValue* args);
   void HandleCreateWalletRequested(const base::ListValue* args);
   void GetWalletProperties(const base::ListValue* args);

   void OnWalletInitialized(brave_rewards::RewardsService* rewards_service,
                            int error_code) override;
   void OnWalletProperties(brave_rewards::RewardsService* rewards_service,
                           int error_code,
                           std::unique_ptr<brave_rewards::WalletProperties> wallet_properties) override;

  DISALLOW_COPY_AND_ASSIGN(RewardsDOMHandler);
};


RewardsDOMHandler::~RewardsDOMHandler() {
  if (rewards_service_)
    rewards_service_->RemoveObserver(this);
}

void RewardsDOMHandler::RegisterMessages() {
  web_ui()->RegisterMessageCallback("brave_rewards_panel.createWalletRequested",
      base::BindRepeating(&RewardsDOMHandler::HandleCreateWalletRequested,
                          base::Unretained(this)));
  web_ui()->RegisterMessageCallback("brave_rewards_panel.getWalletProperties",
      base::BindRepeating(&RewardsDOMHandler::GetWalletProperties,
                          base::Unretained(this)));
  web_ui()->RegisterMessageCallback("brave_rewards_panel.getCurrentWindowId",
      base::BindRepeating(&RewardsDOMHandler::GetCurrentWindowId,
                          base::Unretained(this)));
  web_ui()->RegisterMessageCallback("brave_rewards_panel.getCurrentReport",
                                    base::BindRepeating(&RewardsDOMHandler::GetBalanceReports,
                                                        base::Unretained(this)));
}

void RewardsDOMHandler::GetCurrentWindowId(const base::ListValue* args) {
  if (web_ui()->CanCallJavascript()) {

    //FIX: BrowserList is not available for Android. 
    //Browser* active_window = BrowserList::GetInstance()->GetLastActive();
    //int window_id = active_window->session_id().id();
    int window_id = 0;
    web_ui()->CallJavascriptFunctionUnsafe("brave_rewards_panel.currentWindowId", base::Value(window_id));
  }
}

void RewardsDOMHandler::HandleCreateWalletRequested(const base::ListValue* args) {
  if (!rewards_service_)
    return;

  rewards_service_->CreateWallet();
}

void RewardsDOMHandler::GetWalletProperties(const base::ListValue* args) {
  if (!rewards_service_)
    return;

  rewards_service_->GetWalletProperties();
}

void RewardsDOMHandler::OnWalletInitialized(
    brave_rewards::RewardsService* rewards_service,
    int error_code) {
  if (!web_ui()->CanCallJavascript())
    return;

  if (error_code == 0)
    web_ui()->CallJavascriptFunctionUnsafe("brave_rewards_panel.walletCreated");
  else
    web_ui()->CallJavascriptFunctionUnsafe("brave_rewards_panel.walletCreateFailed");
}

void RewardsDOMHandler::OnWalletProperties(
    brave_rewards::RewardsService* rewards_service,
    int error_code,
    std::unique_ptr<brave_rewards::WalletProperties> wallet_properties) {

  if (web_ui()->CanCallJavascript()) {
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

    web_ui()->CallJavascriptFunctionUnsafe("brave_rewards_panel.walletProperties", result);
  }
}

void RewardsDOMHandler::GetBalanceReports(const base::ListValue* args) {
  if (rewards_service_ && web_ui()->CanCallJavascript()) {
    std::map<std::string, brave_rewards::BalanceReport> reports = rewards_service_->GetAllBalanceReports();

    if (reports.empty()) {
      return;
    }

    base::DictionaryValue newReports;

    for (auto const& report : reports) {
      const brave_rewards::BalanceReport oldReport = report.second;
      auto newReport = std::make_unique<base::DictionaryValue>();
      newReport->SetString("opening", oldReport.opening_balance);
      newReport->SetString("closing", oldReport.closing_balance);
      newReport->SetString("grant", oldReport.grants);
      newReport->SetString("deposit", oldReport.deposits);
      newReport->SetString("ads", oldReport.earning_from_ads);
      newReport->SetString("contribute", oldReport.auto_contribute);
      newReport->SetString("donation", oldReport.recurring_donation);
      newReport->SetString("tips", oldReport.one_time_donation);
      newReport->SetString("total", oldReport.total);
      newReports.SetDictionary(report.first, std::move(newReport));
    }

    web_ui()->CallJavascriptFunctionUnsafe("brave_rewards_panel.balanceReports", newReports);
  }
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
// BraveRewardsPanelUI
//
///////////////////////////////////////////////////////////////////////////////

BraveRewardsPanelUI::BraveRewardsPanelUI(content::WebUI* web_ui, const std::string& name)
    : BasicUI(web_ui, name, chrome::kRewardsPanelJS,
        IDR_BRAVE_REWARDS_PANEL_UI_JS, IDR_BRAVE_REWARDS_PANEL_UI_HTML) {

  auto handler_owner = std::make_unique<RewardsDOMHandler>();
  RewardsDOMHandler * handler = handler_owner.get();
  web_ui->AddMessageHandler(std::move(handler_owner));
  handler->Init();
}

BraveRewardsPanelUI::~BraveRewardsPanelUI() {
}
