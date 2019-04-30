import { h, Component } from 'preact';
import linkState from 'linkstate';
import Button from 'preact-material-components/Button';
import 'preact-material-components/Button/style.css';

import Fab from 'preact-material-components/Fab';
import Icon from 'preact-material-components/Icon';
import 'preact-material-components/Fab/style.css';

import List from 'preact-material-components/List';
import 'preact-material-components/List/style.css';
import 'preact-material-components/Menu/style.css';

import Dialog from 'preact-material-components/Dialog';
import 'preact-material-components/Dialog/style.css';

import TextField from 'preact-material-components/TextField';
import 'preact-material-components/TextField/style.css';

import Switch from 'preact-material-components/Switch';
import 'preact-material-components/Switch/style.css';

import { view } from 'preact-easy-state';

import store from '../../service/store';
import style from './style';

const WiFiNetwork = ({ network, onRemove, onEdit }) => (
  <List.Item selected={true}>
    <List.ItemGraphic>
      <Icon>wifi</Icon>
    </List.ItemGraphic>
    <List.TextContainer>
      <List.PrimaryText>{network.ssid}</List.PrimaryText>
      <List.SecondaryText>{network.dhcp ? 'DHCP' : network.ip}</List.SecondaryText>
    </List.TextContainer>
    <List.ItemMeta>
      <Button onClick={onEdit}>
        <Icon>edit</Icon>
      </Button>
      <Button onClick={onRemove}>
        <Icon>delete</Icon>
      </Button>
    </List.ItemMeta>
  </List.Item>
);

const WiFiNetworks = ({ networks, onRemove, onEdit }) => (
  <List avatar-list={true} two-line={true}>
    {networks.map(
      net => net && <WiFiNetwork key={net.ssid} network={net} onRemove={onRemove(net)} onEdit={onEdit(net)} />,
    )}
  </List>
);

class EditNetwork extends Component {
  state = {
    visible: this.props.visible,
    network: { ...this.props.network },
  };

  componentWillReceiveProps = newProps => {
    console.log('newProps', newProps);
    if (newProps.visible !== this.state.visible && this.dlg) {
      if (newProps.visible) this.dlg.MDComponent.show();
      else this.dlg.MDComponent.close();
    }
    this.setState(state => ({
      ...state,
      visible: newProps.visible,
      network: { ...newProps.network },
    }));
  };

  save = () => {
    const { onSave } = this.props;
    const { network } = this.state;
    onSave(network);
  };

  cancel = () => {
    this.setState(state => ({
      ...state,
      visible: false,
    }));
  };

  render = ({ name }, { network, visible }) => {
    console.log('render', name, network, 'visible', visible);
    return (
      <Dialog ref={dlg => (this.dlg = dlg)}>
        <Dialog.Header>{`${name} WiFi Network`}</Dialog.Header>
        <Dialog.Body>
          <div class={`${style.dialogBody}`}>
            <TextField label="SSID" value={network.ssid} onInput={linkState(this, 'network.ssid')} />
            <TextField label="Password" value={network.pwd} onInput={linkState(this, 'network.pwd')} />
            <div>
              <span>DHCP</span>
              <Switch disabled checked />
            </div>
          </div>
        </Dialog.Body>
        <Dialog.Footer>
          <Dialog.FooterButton cancel={true} onClick={this.save.bind(this)}>
            Ok
          </Dialog.FooterButton>
          <Dialog.FooterButton accept={true} onClick={this.cancel.bind(this)}>
            Cancel
          </Dialog.FooterButton>
        </Dialog.Footer>
      </Dialog>
    );
  };
}

class WiFi extends Component {
  state = {   
    showAddDialog: false,
    showEditDialog: false,
  };

  editWifi = {};

  onShowWifiEdit = net => () => {
    this.editWifi = net;
    this.setState({ showEditDialog: true });
  };

  onShowWifiAdd = () => {
    this.editWifi = {};
    this.setState({ showAddDialog: true });
  };

  onShowWifiRemove = net => () => {
    this.editWifi = net;
    this.removeDlg.MDComponent.show();
  };

  onWifiRemove = () => {
    if (this.editWifi) {
      const { wifi } = store.config;
      for (let i = 0; i < wifi.length; i++) {
        if (wifi[i] && wifi[i].ssid === this.editWifi.ssid) {
          wifi.splice(i,1);
          break;
        }
      }
      this.setState(oldState => ({
        ...oldState,
        showAddDialog: false,
        showEditDialog: false,
      }));
    }
  };

  onWifiSave = net => {
    console.log('NET=', net);
    const { wifi } = store.config;
    for (let i = 0; i < wifi.length; i++) {
      console.log('I=', i, wifi[i]);
      if (wifi[i] && wifi[i]._id === net._id) {
        wifi[i].ssid = net.ssid;
        wifi[i].pwd = net.pwd;
        wifi[i].dhcp = net.dhcp;

        console.log('U=', i, wifi[i]);
        break;
      }
    }

    this.setState(oldState => ({
      ...oldState,
      showAddDialog: false,
      showEditDialog: false,
    }));
  };

  onWifiAdd = net => {
    net._id = store.newId();
      
    store.config.wifi = [...store.config.wifi, net];
    this.setState(oldState => ({
      ...oldState,
      showAddDialog: false,
      showEditDialog: false,
    }));
  };

  render() {
    const { showAddDialog, showEditDialog } = this.state;
    return (
      <div class={`${style.home} page`}>
        <h3>Stored WiFi Networks</h3>
        <WiFiNetworks
          networks={store.config.wifi || []}
          onEdit={this.onShowWifiEdit}
          onRemove={this.onShowWifiRemove}
        />
        <div class={`${style.fabAdd}`}>
          <Fab onClick={this.onShowWifiAdd} primary>
            <Icon>add</Icon>
          </Fab>
        </div>
        <Dialog ref={dlg => (this.removeDlg = dlg)}>
          {/* TODO: WIFI network name */}
          <Dialog.Header>{`Remove WiFi Network?`}</Dialog.Header>
          <Dialog.Body>This operation cannot be undone</Dialog.Body>
          <Dialog.Footer>
            <Dialog.FooterButton cancel={true} onClick={this.onWifiRemove}>
              Ok
            </Dialog.FooterButton>
            <Dialog.FooterButton accept={true}>Cancel</Dialog.FooterButton>
          </Dialog.Footer>
        </Dialog>
        <EditNetwork name="Edit" network={this.editWifi} onSave={this.onWifiSave.bind(this)} visible={showEditDialog} />
        <EditNetwork name="Add" network={{}} onSave={this.onWifiAdd.bind(this)} visible={showAddDialog} />
      </div>
    );
  }
}

export default view(WiFi);
