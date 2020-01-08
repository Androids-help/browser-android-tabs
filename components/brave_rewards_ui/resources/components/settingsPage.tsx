/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import { bindActionCreators, Dispatch } from 'redux'
import { connect } from 'react-redux'

// Components
import AdsBox from './adsBox'
import ContributeBox from './contributeBox'
import DonationBox from './donationsBox'
import {
  MainToggleMobile,
  SettingsPageMobile,
  WalletInfoHeader
} from 'brave-ui/features/rewards/mobile'
import { StyledDisabledContent, StyledHeading, StyledText } from './style'

// Utils
import * as utils from '../utils'
import * as rewardsActions from '../actions/rewards_actions'

interface State {
  mainToggle: boolean
}

interface Props extends Rewards.ComponentProps {
  rewardsEnabled?: boolean
}

class SettingsPage extends React.Component<Props, State> {
  private balanceTimerId: number

  constructor (props: Props) {
    super(props)
    this.state = {
      mainToggle: true
    }
  }

  onToggle = () => {
    this.setState({ mainToggle: !this.state.mainToggle })
    this.actions.onSettingSave('enabledMain', !this.props.rewardsData.enabledMain)
  }

  get actions () {
    return this.props.actions
  }

  componentDidMount () {
    if (this.props.rewardsData.firstLoad === null) {
      // First load ever
      this.actions.onSettingSave('firstLoad', true)
      this.actions.getWalletPassphrase()
    } else if (this.props.rewardsData.firstLoad) {
      // Second load ever
      this.actions.onSettingSave('firstLoad', false)
    }

    this.actions.getWalletProperties()
    this.balanceTimerId = setInterval(() => {
      this.actions.getWalletProperties()
    }, 60000)

    this.actions.getGrant()
  }

  componentWillUnmount () {
    clearInterval(this.balanceTimerId)
  }

  render () {
    const { enabledMain } = this.props.rewardsData
    const { balance, rates } = this.props.rewardsData.walletInfo
    const convertedBalance = utils.convertBalance(balance.toString(), rates)

    return (
      <SettingsPageMobile>
        <MainToggleMobile
          onToggle={this.onToggle}
          enabled={enabledMain}
        />
        {
          !this.state.mainToggle
          ? <StyledDisabledContent>
              <StyledHeading>
                {'Why Brave Rewards?'}
              </StyledHeading>
              <StyledText>
                {'With conventional browsers, you pay to browse the web by viewing ads with your valuable attention, spending your valuable time downloading invasive ad technology, that transmits your valuable private data to advertisers — without your consent.'}
              </StyledText>
              <StyledText>
                {'Today, Brave welcomes you to the new internet. One where your time is valued, your personal data is kept private, and you actually get paid for your attention.'}
              </StyledText>
            </StyledDisabledContent>
          : null
        }
        <WalletInfoHeader
          balance={balance.toString()}
          id={'mobile-wallet'}
          converted={convertedBalance}
        />
        <AdsBox/>
        <ContributeBox
          enabledContribute={enabledMain}
        />
        <DonationBox/>
      </SettingsPageMobile>
    )
  }
}

const mapStateToProps = (state: Rewards.ApplicationState) => ({
  rewardsData: state.rewardsData
})

const mapDispatchToProps = (dispatch: Dispatch) => ({
  actions: bindActionCreators(rewardsActions, dispatch)
})

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(SettingsPage)
