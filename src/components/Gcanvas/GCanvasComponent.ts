import codegenNativeComponent from "react-native/Libraries/Utilities/codegenNativeComponent"
import {
    HostComponent,
    ViewProps,
  } from 'react-native'
  import {DirectEventHandler} from "react-native/Libraries/Types/CodegenTypes";  


export interface NativeGCanvasProps extends ViewProps {
  onChange?: DirectEventHandler<undefined>; 
}
  
export default codegenNativeComponent<NativeGCanvasProps>("RNCGCanvasView") as HostComponent<NativeGCanvasProps>; 