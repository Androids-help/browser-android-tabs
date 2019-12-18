/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import { bindActionCreators, Dispatch } from 'redux'
import { connect } from 'react-redux'
import { PanelWelcome } from 'brave-ui/features/rewards'

// Components
import Panel from './panel'

// Utils
import * as rewardsPanelActions from '../actions/rewards_panel_actions'

interface Props extends RewardsExtension.ComponentProps {
}

interface State {
  windowId: number
}

export class RewardsPanel extends React.Component<Props, State> {
  constructor (props: Props) {
    super(props)
    this.state = {
      windowId: -1
    }
  }

  componentDidMount () {
    this.props.actions.getCurrentWindowId()
  }

  render () {
    const { rewardsPanelData, actions } = this.props

    const walletCreated = rewardsPanelData.walletCreated || false
    const windowId = rewardsPanelData.currentWindowId === this.state.windowId
      ? this.state.windowId
      : rewardsPanelData.currentWindowId
    return (
      <>
        {
          !walletCreated
          ? <PanelWelcome
            variant={'two'}
            optInErrorAction={actions.onWalletCreateFailed}
            optInAction={actions.createWallet}
          />
          : <Panel windowId={windowId || -1} />
        }
      </>
    )
  }
}

export const mapStateToProps = (state: RewardsExtension.ApplicationState) => ({
  rewardsPanelData: state.rewardsPanelData
})

export const mapDispatchToProps = (dispatch: Dispatch) => ({
  actions: bindActionCreators(rewardsPanelActions, dispatch)
})

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(RewardsPanel)
