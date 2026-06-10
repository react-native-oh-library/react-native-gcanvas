import {Platform, requireNativeComponent} from 'react-native';
import {GCanvas} from './Gcanvas';
const CanvasView = Platform.select({
  ios: requireNativeComponent('RCTGCanvasView', null, {
    nativeOnly: {onChange: true},
  }),
  android: requireNativeComponent('GCanvasView', null, {
    nativeOnly: {onChange: true},
  }),
  harmony: GCanvas
});
export default CanvasView;
