import { h, Component } from 'preact';
import List from 'preact-material-components/List';
import Icon from 'preact-material-components/Icon';
import Button from 'preact-material-components/Button';
import TextField from 'preact-material-components/TextField';
import 'preact-material-components/Button/style.css';
import 'preact-material-components/Card/style.css';
import 'preact-material-components/Button/style.css';
import 'preact-material-components/List/style.css';
import 'preact-material-components/Menu/style.css';
import 'preact-material-components/Select/style.css';
import 'preact-material-components/TextField/style.css';

import style from './style.css';
import store from '../../service/store';

class NumberText extends Component {
  state = {
    valid: true,
  };

  onChange = fn => e => {
    const x = Number(e.target.valueAsNumber);
    console.log('change:', x, e.target);
    const valid = this.props.min <= x && this.props.max >= x;
    this.setState({ value: e.target.value, valid: valid, message: valid ? null : 'Invalid!' });
    if (valid && fn) {
      fn(e);
    }
  };

  render = ({ helperText, label, onChange }, { valid, value, message }) => {
    return (
      <TextField
        type="number"
        helperText={message || helperText}
        label={label}
        helperTextPersistent
        onChange={this.onChange(onChange)}
        invalid={message === null}
        required
        value={value}
      />
    );
  };
}

const pinTypes = {
  0: 'Disabled',
  1: 'Relay',
  2: 'PWM',
};

const getPinType = type => {
  return pinTypes[type] || 'Unknown';
};

const getPinDetails = pin => {
  const type = <span>{getPinType(pin.type)}</span>;
  switch (pin.type) {
    case 0:
      return null;
    case 1:
      return (
        <span>
          {type}&nbsp;
          <span>threshold:&nbsp;</span>
          <span>{pin.threshold}</span>
        </span>
      );
    case 2:
      return (
        <span>
          {type}&nbsp;
          <span>pulse:&nbsp;</span>
          <span>{pin.pulse}</span>
        </span>
      );
  }
  return;
};

const Pin = ({ pin, onRemove, onEdit }) => (
  <List.Item selected={true}>
    <List.ItemGraphic>
      <Icon>wifi</Icon>
    </List.ItemGraphic>
    <List.TextContainer>
      <List.PrimaryText>
        <span>GPIO&nbsp;{pin.pin}</span>&nbsp;<span>DMX: {pin.channel}</span>
      </List.PrimaryText>
      <List.SecondaryText>{getPinDetails(pin)}</List.SecondaryText>
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

const Pins = ({ pins, onRemove, onEdit }) => (
  <List avatar-list={true} two-line={true}>
    {pins.map(
      pin => pin && <Pin key={pin.pin} pin={pin} onRemove={onRemove && onRemove(pin)} onEdit={onEdit && onEdit(pin)} />,
    )}
  </List>
);

export default class DMX extends Component {
  render() {
    const { dmx } = store.config;
    console.log('dmx:', dmx);
    return (
      <div class={`${style.home} page`}>
        <h3>GPIO Pins</h3>
        <h5>
          <span class={style.warning}>WARNING:</span>&nbsp; 
          incorrect settings may cause a physical damage to your
          device or represent a fire hazard.
        </h5>
        <h5>Relay: [Requires 1 DMX channel] switches ON or OFF when a value crosses the threshold</h5>
        <h5>PWM: [Requires 2 DMX channels] first channel sets the brightness, the second sets the strobe interval </h5>
        <Pins pins={dmx} />
      </div>
    );
  }
}
