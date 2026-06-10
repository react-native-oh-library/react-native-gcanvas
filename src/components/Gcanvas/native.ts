import React, {Component, ReactNode} from 'react';
import { NativeGCanvasProps } from './GCanvasComponent';
import  RNCGCanvas from './GCanvasComponent';

const NativeGCanvas = RNCGCanvas;

type NativeGCanvasInstance = InstanceType<typeof NativeGCanvas>;


class GCanvas extends Component<NativeGCanvasProps> {
  _nativeRef: NativeGCanvasInstance | null = null;

  _onChange = () => {
    this.props?.onChange?.();
    this.forceUpdate();
  };

  _setNativeRef = (ref: NativeGCanvasInstance) => {
    this._nativeRef = ref;
  };

  render(): ReactNode {
    return React.createElement(
      NativeGCanvas,
      {
        ...this.props,
        ref: this._setNativeRef,
        onChange: this._onChange
      }
    );
  }
}

export {GCanvas};
