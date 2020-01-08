/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_REWARDS_NATIVE_WORKER_H_
#define BRAVE_REWARDS_NATIVE_WORKER_H_

#include <jni.h>
#include <memory>
#include <map>
#include "base/android/jni_weak_ref.h"
#include "base/memory/weak_ptr.h"
#include "brave/components/brave_rewards/browser/balance_report.h"
#include "brave/components/brave_rewards/browser/rewards_service_observer.h"
#include "brave/components/brave_rewards/browser/rewards_notification_service_observer.h"
#include "brave/components/brave_rewards/browser/rewards_service_private_observer.h"
#include "brave/components/brave_rewards/browser/wallet_properties.h"
#include "brave/vendor/bat-native-ledger/include/bat/ledger/publisher_info.h"

namespace brave_rewards {
  class RewardsService;
}

namespace chrome {
namespace android {

class BraveRewardsNativeWorker : public brave_rewards::RewardsServiceObserver,
    public brave_rewards::RewardsServicePrivateObserver,
    public brave_rewards::RewardsNotificationServiceObserver {
public:
    BraveRewardsNativeWorker(JNIEnv* env, const base::android::JavaRef<jobject>& obj);
    ~BraveRewardsNativeWorker() override;

    void Destroy(JNIEnv* env, const
        base::android::JavaParamRef<jobject>& jcaller);

    void CreateWallet(JNIEnv* env, const
        base::android::JavaParamRef<jobject>& jcaller);

    void WalletExist(JNIEnv* env, const
        base::android::JavaParamRef<jobject>& jcaller);

    void GetWalletProperties(JNIEnv* env, const
        base::android::JavaParamRef<jobject>& jcaller);

    void GetPublisherInfo(JNIEnv* env, const
        base::android::JavaParamRef<jobject>& jcaller, int tabId,
        const base::android::JavaParamRef<jstring>& host);

    double GetWalletBalance(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj);

    double GetWalletRate(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj,
        const base::android::JavaParamRef<jstring>& rate);

    base::android::ScopedJavaLocalRef<jstring> GetPublisherURL(JNIEnv* env, 
        const base::android::JavaParamRef<jobject>& obj, uint64_t tabId);

    base::android::ScopedJavaLocalRef<jstring> GetPublisherFavIconURL(JNIEnv* env, 
        const base::android::JavaParamRef<jobject>& obj, uint64_t tabId);

    base::android::ScopedJavaLocalRef<jstring> GetPublisherName(JNIEnv* env, 
        const base::android::JavaParamRef<jobject>& obj, uint64_t tabId);

    base::android::ScopedJavaLocalRef<jstring> GetPublisherId(JNIEnv* env, 
        const base::android::JavaParamRef<jobject>& obj, uint64_t tabId);

    int GetPublisherPercent(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj,
        uint64_t tabId);

    bool GetPublisherExcluded(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj,
        uint64_t tabId);

    bool GetPublisherVerified(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj,
        uint64_t tabId);

    void GetCurrentBalanceReport(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj);

    void IncludeInAutoContribution(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj,
        uint64_t tabId, bool exclude);

    void RemovePublisherFromMap(JNIEnv* env, 
        const base::android::JavaParamRef<jobject>& obj, uint64_t tabId);

    void Donate(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj,
        const base::android::JavaParamRef<jstring>& publisher_key, int amount, bool recurring);

    void GetAllNotifications(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj);

    void DeleteNotification(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj,
        const base::android::JavaParamRef<jstring>& notification_id);

    void GetGrant(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj);

    int GetCurrentGrantsCount(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj);
    base::android::ScopedJavaLocalRef<jobjectArray> GetCurrentGrant(JNIEnv* env, 
        const base::android::JavaParamRef<jobject>& obj,
        int position);

    void GetPendingContributionsTotal(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj);
    void OnGetPendingContributionsTotal(double amount);

    void OnIsWalletCreated(bool created);


    void OnWalletInitialized(brave_rewards::RewardsService* rewards_service,
        int error_code) override;

    void OnWalletProperties(brave_rewards::RewardsService* rewards_service,
        int error_code, 
        std::unique_ptr<brave_rewards::WalletProperties> wallet_properties) override;

    void OnPanelPublisherInfo(
        brave_rewards::RewardsService* rewards_service,
        int error_code,
        std::unique_ptr<ledger::PublisherInfo> info,
        uint64_t tabId) override;

    void OnGetCurrentBalanceReport(brave_rewards::RewardsService* rewards_service,
        const brave_rewards::BalanceReport& balance_report) override;

    void OnNotificationAdded(
      brave_rewards::RewardsNotificationService* rewards_notification_service,
      const brave_rewards::RewardsNotificationService::RewardsNotification& notification) override;

    void OnGetAllNotifications(
      brave_rewards::RewardsNotificationService* rewards_notification_service,
      const brave_rewards::RewardsNotificationService::RewardsNotificationsList&
          notifications_list) override;

    void OnNotificationDeleted(
      brave_rewards::RewardsNotificationService* rewards_notification_service,
      const brave_rewards::RewardsNotificationService::RewardsNotification& notification) override;

    void OnGrant(brave_rewards::RewardsService* rewards_service, unsigned int result,
        brave_rewards::Grant grant) override;

private:
    JavaObjectWeakGlobalRef weak_java_brave_rewards_native_worker_;
    brave_rewards::RewardsService* brave_rewards_service_;
    brave_rewards::WalletProperties wallet_properties_;
    std::map<uint64_t, ledger::PublisherInfo> map_publishers_info_;

    base::WeakPtrFactory<BraveRewardsNativeWorker> weak_factory_;
};
}
}

#endif // BRAVE_REWARDS_NATIVE_WORKER_H_
