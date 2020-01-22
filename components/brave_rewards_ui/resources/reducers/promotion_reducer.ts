/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import { Reducer } from 'redux'

// Constant
import { types } from '../constants/rewards_types'

const getGrant = (id?: string, grants?: Rewards.Grant[]) => {
  if (!id || !grants) {
    return null
  }

  return grants.find((grant: Rewards.Grant) => {
    return (grant.promotionId === id)
  })
}

const updateGrant = (newGrant: Rewards.Grant, grants: Rewards.Grant[]) => {
  return grants.map((grant: Rewards.Grant) => {
    if (newGrant.promotionId === grant.promotionId) {
      return Object.assign(grant, newGrant)
    }
    return grant
  })
}


const grantReducer: Reducer<Rewards.State | undefined> = (state: Rewards.State, action) => {
  switch (action.type) {
    case types.FETCH_PROMOTIONS: {
      chrome.send('brave_rewards.fetchPromotions')
      break
    }
    case types.ON_PROMOTIONS: {
      state = { ...state }
      const grantProps = action.payload.properties

      if (grantProps.result === 1) {
        break
      }


      if (!state.grants) {
        state.grants = []
      }

      let localGrants = [];
      for (let promo of grantProps.promotions) {
      	if (promo.status == 4){ //skip FINISHED
	  continue;
	}

	const grantType = (promo.type == 0)? 'ugp' : 'ads'
	localGrants.push({
          promotionId: promo.promotionId,
          expiryTime: 0,
          probi: '',
          type: grantType
	});
     }


      state = {
        ...state,
        grants: localGrants
      }

      break
    }
    case types.CLAIM_PROMOTION:
      const promotionId = action.payload.promotionId
      if (!promotionId) {
        break
      }

      if (!state.grants) {
        break
      }

      const currentGrant = getGrant(promotionId, state.grants)

      if (!currentGrant) {
        break
      }

      state = {
        ...state,
        currentGrant
      }

      chrome.send('brave_rewards.claimPromotion', [currentGrant.promotionId])
      break
    case types.SOLVE_GRANT_CAPTCHA: {
      const promotionId = state.currentGrant && state.currentGrant.promotionId

      if (promotionId && action.payload.x && action.payload.y) {
        chrome.send('brave_rewards.solveGrantCaptcha', [JSON.stringify({
          x: action.payload.x,
          y: action.payload.y
        }), promotionId])
      }
      break
    }
    case types.ON_GRANT_RESET: {
      if (state.currentGrant && state.grants) {
        let currentGrant: any = state.currentGrant

        const grants = state.grants.map((item: Rewards.Grant) => {
          if (currentGrant.promotionId === item.promotionId) {
            return {
              promotionId: currentGrant.promotionId,
              probi: '',
              expiryTime: 0,
              type: currentGrant.type
            }
          }
          return item
        })

        currentGrant = undefined

        state = {
          ...state,
          grants,
          currentGrant
        }
      }
      break
    }
    case types.DELETE_PROMOTION: {
      if (state.currentGrant && state.grants) {
        let grantIndex = -1
        let currentGrant: any = state.currentGrant

        state.grants.map((item: Rewards.Grant, i: number) => {
          if (currentGrant.promotionId === item.promotionId) {
            grantIndex = i
          }
        })

        if (grantIndex > -1) {
          state.grants.splice(grantIndex, 1)
          currentGrant = undefined
        }

        state = {
          ...state,
          currentGrant
        }
      }
      break
    }
    case types.ON_PROMOTION_FINISH: {
      state = { ...state }
      let newGrant: any = {}
      const properties: Rewards.PromotionFinish = action.payload.properties

      if (!state.grants) {
        break
      }

      newGrant.promotionId = properties.promotion && properties.promotion.promotionId

      if (!newGrant.promotionId) {
        break
      }

      const promotion = properties.promotion

      switch (properties.result) {
        case 0:
          let ui = state.ui
          newGrant.expiryTime = (promotion !== undefined)?  promotion.expiresAt : 0
          newGrant.probi = (promotion !== undefined)? promotion.amount : 0
          newGrant.status = null
          ui.emptyWallet = false

          state = {
            ...state,
            ui
          }

          chrome.send('brave_rewards.getWalletProperties')
          break
        default:
          newGrant.status = 'generalError'
          break
      }

      if (state.grants) {
        const grants = updateGrant(newGrant, state.grants)

        state = {
          ...state,
          grants
        }
      }

      break
    }
  }

  return state
}

export default grantReducer
