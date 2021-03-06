// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.preferences;

/**
 * A prefix for a range of SharedPreferences keys generated dynamically.
 *
 * Instances should be declared as keys in {@link ChromePreferenceKeys}.
 */
public class KeyPrefix {
    private final String mPrefix;

    KeyPrefix(String pattern) {
        // More thorough checking is performed in ChromePreferenceKeysTest.
        assert pattern.endsWith(".*");
        mPrefix = pattern.substring(0, pattern.length() - 1);
    }

    /**
     * @param suffix A non-empty string. The '*' character is reserved.
     * @return The complete SharedPreferences key to be passed to {@link SharedPreferencesManager}.
     */
    public String createKey(String suffix) {
        return mPrefix + suffix;
    }

    String pattern() {
        return mPrefix + "*";
    }
}
