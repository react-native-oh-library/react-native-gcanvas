import { TurboModulesFactory } from '@rnoh/react-native-openharmony/ts';

import { RNOHPackage, UITurboModule, UITurboModuleContext } from '@rnoh/react-native-openharmony';

import { GCanvasModule } from './GCanvasModule'

class ReanimatedTurboModulesFactory extends TurboModulesFactory {
  createTurboModule(name: string): UITurboModule | null {
    if (name === GCanvasModule.NAME) {
      console.log("GCanvas ets GCanvasModule")
      return new GCanvasModule(this.ctx);
    }
    return null;
  }

  hasTurboModule(name: string): boolean {
    return name === 'GCanvasModule';
  }
}

export class RNCGCanvasPackage extends RNOHPackage {
  createTurboModulesFactory(ctx: UITurboModuleContext): TurboModulesFactory {
    return new ReanimatedTurboModulesFactory(ctx);
  }
}