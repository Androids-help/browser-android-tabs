// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Include test fixture.
GEN_INCLUDE([
  '//chrome/browser/resources/chromeos/accessibility/chromevox/testing/chromevox_next_e2e_test_base.js',
  '//chrome/browser/resources/chromeos/accessibility/chromevox/testing/assert_additions.js'
]);

/**
 * Test fixture for recovery strategy tests.
 * @constructor
 * @extends {ChromeVoxNextE2ETest}
 */
function ChromeVoxRecoveryStrategyTest() {
  ChromeVoxNextE2ETest.call(this);
  window.RoleType = chrome.automation.RoleType;
}

ChromeVoxRecoveryStrategyTest.prototype = {
  __proto__: ChromeVoxNextE2ETest.prototype,
};

TEST_F('ChromeVoxRecoveryStrategyTest', 'ReparentedRecovery', function() {
  this.runWithLoadedTree(
      `
    <input type="text"></input>
    <p id="p">hi</p>
    <button id="go"</button>
    <script>
      document.getElementById('go').addEventListener('click', function() {
        var p = document.getElementById('p');
        p.remove();
        document.body.appendChild(p);
      });
    </script>
  `,
      function(root) {
        var p = root.find({role: RoleType.PARAGRAPH});
        var s = root.find({role: RoleType.STATIC_TEXT});
        var b = root.find({role: RoleType.BUTTON});
        var bAncestryRecovery = new AncestryRecoveryStrategy(b);
        var pAncestryRecovery = new AncestryRecoveryStrategy(p);
        var sAncestryRecovery = new AncestryRecoveryStrategy(s);
        var bTreePathRecovery = new TreePathRecoveryStrategy(b);
        var pTreePathRecovery = new TreePathRecoveryStrategy(p);
        var sTreePathRecovery = new TreePathRecoveryStrategy(s);
        this.listenOnce(b, 'clicked', function() {
          assertFalse(
              bAncestryRecovery.requiresRecovery(),
              'bAncestryRecovery.requiresRecovery');
          assertTrue(
              pAncestryRecovery.requiresRecovery(),
              'pAncestryRecovery.requiresRecovery()');
          assertTrue(
              sAncestryRecovery.requiresRecovery(),
              'sAncestryRecovery.requiresRecovery()');
          assertFalse(
              bTreePathRecovery.requiresRecovery(),
              'bTreePathRecovery.requiresRecovery()');
          assertTrue(
              pTreePathRecovery.requiresRecovery(),
              'pTreePathRecovery.requiresRecovery()');
          assertTrue(
              sTreePathRecovery.requiresRecovery(),
              'sTreePathRecovery.requiresRecovery()');

          assertEquals(RoleType.BUTTON, bAncestryRecovery.node.role);
          assertEquals(root, pAncestryRecovery.node);
          assertEquals(root, sAncestryRecovery.node);

          assertEquals(b, bTreePathRecovery.node);
          assertEquals(b, pTreePathRecovery.node);
          assertEquals(b, sTreePathRecovery.node);

          assertFalse(
              bAncestryRecovery.requiresRecovery(),
              'bAncestryRecovery.requiresRecovery()');
          assertFalse(
              pAncestryRecovery.requiresRecovery(),
              'pAncestryRecovery.requiresRecovery()');
          assertFalse(
              sAncestryRecovery.requiresRecovery(),
              'sAncestryRecovery.requiresRecovery()');
          assertFalse(
              bTreePathRecovery.requiresRecovery(),
              'bTreePathRecovery.requiresRecovery()');
          assertFalse(
              pTreePathRecovery.requiresRecovery(),
              'pTreePathRecovery.requiresRecovery()');
          assertFalse(
              sTreePathRecovery.requiresRecovery(),
              'sTreePathRecovery.requiresRecovery()');
        });
        // Trigger the change.
        b.doDefault();
      });
});
