'use strict';
import { TurboModule, TurboModuleRegistry } from 'react-native';


// RNOH patch
interface Spec extends TurboModule {
    enable(config: { componentId: string; config: any[] }): any;
    disable(componentId: string): void;
    setContextType(contextType: number, componentId: string): void;
    setDevicePixelRatio(componentId: string, ratio: number): void;
    resetGlViewport(componentId: string): void;
    toDataURL(componentId: string, mimeType: string, quality: number): string;
    resetComponent?(componentId: string): void;
    extendCallNative(config: { contextId: string; args: string; type: number }): { result?: any };
    render(componentId: string, commands: string, type: number): void;
    texImage2D(componentId: string, target: number, level: number, internalformat: number, format: number, type: number, src: string): void;
    texSubImage2D(componentId: string, target: number, level: number, xoffset: number, yoffset: number, format: number, type: number, src: string): void;
    drawCanvas2Canvas(map: any): void;
    bindImageTexture(config: [string, string], componentId: string, callback: (e: any) => void): void;
    preLoadImage(config: [string, string], callback: (image: any) => void): void;
}

export default TurboModuleRegistry.get<Spec>('GCanvasModule') as Spec | null;
