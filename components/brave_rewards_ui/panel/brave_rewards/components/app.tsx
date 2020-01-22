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
  creating: boolean
}

export class RewardsPanel extends React.Component<Props, State> {
  constructor (props: Props) {
    super(props)
    this.state = {
      windowId: -1,
      creating: false
    }
  }

  get actions () {
    return this.props.actions
  }

  getTabData () {
    // ToDo, replace chrome.query
  }

  openRewardsPage () {
    window.open('chrome://rewards')
  }

  onCreate = () => {
    this.setState({
      creating: true
    })
    this.actions.createWallet()
  }

  componentDidUpdate = (prevProps: Props, prevState: State) => {
    if (
      !prevProps.rewardsPanelData.walletCreated &&
      this.props.rewardsPanelData.walletCreated
    ) {
      this.getTabData()
    }

    if (
      this.state.creating &&
      !prevProps.rewardsPanelData.walletCreateFailed &&
      this.props.rewardsPanelData.walletCreateFailed
    ) {
      this.setState({
        creating: false
      })
    }
  }

  render () {
    const { rewardsPanelData, actions } = this.props
    const walletCreated = rewardsPanelData.walletCreated || false

    return (
      <>
        {
          !walletCreated
          ? <PanelWelcome
            variant={'two'}
            moreLink={this.openRewardsPage}
            creating={this.state.creating}
            optInErrorAction={actions.onWalletCreateFailed}
            optInAction={this.onCreate}
          />
          : <Panel windowId={-1} />
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
