/*
 * If radio not already configured, make it so.
 * And insure radio is ready (state==disabled)
 *
 * In NEW design, the radio stays configured
 */
/* OLD
void Network::prepareToTransmitOrReceive() {
	if (!radio->isConfigured()) {
			radio->resetAndConfigure();
			// TESTING: lower xmit power 8
			// radio->configureXmitPower(8);
		}
	assert(!radio->isInUse());
}
*/
