import { h, Component } from 'preact';
import { route } from 'preact-router';
import TopAppBar from 'preact-material-components/TopAppBar';
import Drawer from 'preact-material-components/Drawer';
import List from 'preact-material-components/List';
import Button from 'preact-material-components/Button';
import Icon from 'preact-material-components/Icon';
import Dialog from 'preact-material-components/Dialog';
import 'preact-material-components/Button/style.css';
import 'preact-material-components/Drawer/style.css';
import 'preact-material-components/List/style.css';
import 'preact-material-components/TopAppBar/style.css';
import 'preact-material-components/Dialog/style.css';

import store from '../../service/store';

export default class Header extends Component {
  closeDrawer() {
    this.drawer.MDComponent.open = false;
    this.state = {
      darkThemeEnabled: false,
    };
  }

  openDrawer = () => (this.drawer.MDComponent.open = true);

  drawerRef = drawer => (this.drawer = drawer);

  openRestart = () => {
    this.restartDlg.MDComponent.show();
  };

  linkTo = path => () => {
    route(path);
    this.closeDrawer();
  };

  restartRef = restartDlg => (this.restartDlg = restartDlg);

  onRestart = () => {
    store.restart();
  };

  onSave = () => {
    store.save();
  };

  goHome = this.linkTo('/');
  goToMyProfile = this.linkTo('/profile');
  goToNetwork = this.linkTo('/net');
  goToWiFi = this.linkTo('/wifi');
  goToHW = this.linkTo('/hardware');
  goToDMX = this.linkTo('/dmx');

  render(props) {
    console.log(props.selectedRoute);
    return (
      <div>
        <TopAppBar className="topappbar">
          <TopAppBar.Row>
            <TopAppBar.Section align-start>
              <TopAppBar.Icon menu onClick={this.openDrawer}>
                menu
              </TopAppBar.Icon>
              <TopAppBar.Title>ArtNet Node</TopAppBar.Title>
            </TopAppBar.Section>
            <TopAppBar.Section align-end>
              <Button raised secondary onClick={this.onSave}>
                <Icon>save</Icon>
              </Button>
              <Button raised primary onClick={this.openRestart}>
                <Icon>sync</Icon>
              </Button>
            </TopAppBar.Section>
          </TopAppBar.Row>
        </TopAppBar>
        <Drawer modal ref={this.drawerRef}>
          <Drawer.DrawerContent>
            <div tabIndex={0}>
              <Drawer.DrawerItem selected={props.selectedRoute === '/'} onClick={this.goHome}>
                <List.ItemGraphic>home</List.ItemGraphic>
                Dashboard
            </Drawer.DrawerItem>
              {/*             <Drawer.DrawerItem selected={props.selectedRoute === '/net'} onClick={this.goToNetwork}>
              <List.ItemGraphic>settings_ethernet</List.ItemGraphic>
              Network
            </Drawer.DrawerItem> */}
              <Drawer.DrawerItem selected={props.selectedRoute === '/wifi'} onClick={this.goToWiFi}>
                <List.ItemGraphic>wifi</List.ItemGraphic>
                WiFi
            </Drawer.DrawerItem>
              <Drawer.DrawerItem selected={props.selectedRoute === '/dmx'} onClick={this.goToDMX}>
                <List.ItemGraphic>settings_remote</List.ItemGraphic>
                DMX
            </Drawer.DrawerItem>
              <Drawer.DrawerItem selected={props.selectedRoute === '/hardware'} onClick={this.goToHW}>
                <List.ItemGraphic>developer_board</List.ItemGraphic>
                Board
            </Drawer.DrawerItem>
            </div>
          </Drawer.DrawerContent>
        </Drawer>

        <Dialog ref={this.restartRef}>
          <Dialog.Header>Restart?</Dialog.Header>
          <Dialog.Body>
            The device will restart and apply the saved settings. This may take up to a minute to complete.
          </Dialog.Body>
          <Dialog.Footer>
            <Dialog.FooterButton cancel={true}>Cancel</Dialog.FooterButton>
            <Dialog.FooterButton primary accept={true} onClick={this.onRestart}>
              Restart
            </Dialog.FooterButton>
          </Dialog.Footer>
        </Dialog>
      </div>
    );
  }
}
