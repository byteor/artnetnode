import { h, Component } from 'preact';
import { Router, route } from 'preact-router';
import { view } from 'preact-easy-state';

import Header from './header';
import Home from '../routes/home';
import NotFound from '../routes/404';
import WiFi from '../routes/wifi';
import Network from '../routes/network';
import Hardware from '../routes/hardware';
import DMX from '../routes/dmx';
import Progress from '../components/progress';
import store from './../service/store';

class App extends Component {
  /** Gets fired when the route changes.
   *	@param {Object} event		"change" event from [preact-router](http://git.io/preact-router)
   *	@param {string} event.url	The newly routed URL
   */
  handleRoute = e => {
    if (e.url === '/index.html') {
      route('/wifi', true);
    } else {
      this.setState({
        currentUrl: e.url,
      });
    }
  };

  componentDidMount = () => {
    (async () => {
      await store.load();
    })().catch(error => console.log('error:', error));
  };

  render() {
    return (
      <div id="app">
        <Progress show={store.isLoading} />
        <Header selectedRoute={this.state.currentUrl} />
        <Router onChange={this.handleRoute}>
          <Home path="/" />
          {/* <Network path="/net" /> */}
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
