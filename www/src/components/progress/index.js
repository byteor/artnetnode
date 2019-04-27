import style from './style.css';

export default ({show}) => {
  return show ? (
    <div class={`${style.loader}`}>
      <div class={`${style['la-ball-atom']} ${style['la-2x']}`}>
        <div />
        <div />
        <div />
        <div />
      </div>
    </div>
  ) : null;
};
