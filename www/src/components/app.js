import { h, Component } from 'preact';
import { Router } from 'preact-router';
import { view } from 'preact-easy-state';

import Header from './header';
import Home from '../routes/home';
import Profile from '../routes/profile';
import NotFound from '../routes/404';
import WiFi from '../routes/wifi';
import Network from '../routes/network';
import Hardware from '../routes/hardware';
import DMX from '../routes/dmx';
import store from './../service/store';

class App extends Component {
	/** Gets fired when the route changes.
	 *	@param {Object} event		"change" event from [preact-router](http://git.io/preact-router)
	 *	@param {string} event.url	The newly routed URL
	 */
	handleRoute = e => {
		this.setState({
			currentUrl: e.url
		});
	};

	render() {
		return (
			<div id="app">
				<Header selectedRoute={this.state.currentUrl} />
				<Router onChange={this.handleRoute}>
					<Home path="/" />
					<Network path="/net" />
					<WiFi path="/wifi" />
					<Hardware path="/hardware" />
					<DMX path="/dmx" />
					<NotFound default />
				</Router>
			</div>
		);
	}
}

export default view(App);