/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

 import BigNumber from 'bignumber.js'

 export function convertProbiToFixed (probi: string, places: number = 1) {
  const result = new BigNumber(probi).dividedBy('1e18').toFixed(places, BigNumber.ROUND_DOWN)

  if (result === 'NaN') {
    return '0.0'
  }

  return result
}