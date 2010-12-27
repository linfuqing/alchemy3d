package AlchemyLib.lights
{
	import AlchemyLib.base.Library;
	import AlchemyLib.base.Node;
	import AlchemyLib.container.Entity;
	
	import flash.geom.ColorTransform;

	public class Light3D extends Node
	{
		//public var pointer:uint = 0;
		
		private var         modePtr:uint;
		private var       bOnOffPtr:uint;
		private var         typePtr:uint;
		private var      ambientPtr:uint;
		private var      diffusePtr:uint;
		private var     specularPtr:uint;
		private var        rangePtr:uint;
		private var      falloffPtr:uint;
		private var        thetaPtr:uint;
		private var          phiPtr:uint;
		private var attenuation0Ptr:uint;
		private var attenuation1Ptr:uint;
		private var attenuation2Ptr:uint;
		
		private var _mode:int;			//光照模式
		private var _bOnOff:Boolean;    //灯光是否开启
		private var _type:int;			//定义灯光类型，我们能够使用下面三种类型之一：POINT_LIGHT, SPOT_LIGHT, DIRECTIONAL_LIGHT
		
		private var _ambient:ColorTransform;		//此光源发出的环境光颜色
		private var _diffuse:ColorTransform;		//此光源发出的漫射光颜色
		private var _specular:ColorTransform;		//此光源发出的镜面光颜色
		
		private var _range:Number;		//灯光能够传播的最大范围
		private var _falloff:Number;	//这个值只能用在聚光灯上。它定义灯光在从内圆锥到外圆锥之间的强度衰减。它的值通常设置为1.0f
		private var _theta:Number;		//只用于聚光灯；指定内圆锥的角度，单位是弧度
		private var _phi:Number;		//只用于聚光灯；指定外圆锥的角度，单位是弧度
		
		//这些衰减变量被用来定义灯光强度的传播距离衰减
		private var _attenuation0:Number;
		private var _attenuation1:Number;
		private var _attenuation2:Number;
		
		private var _source:Entity;
		
		public function get mode():int
		{
			return this._mode;
		}
		
		public function set mode(mode:int):void
		{
			//if (!checkInitialized()) return;
			
			this._mode = mode;
			
			Library.memory.position = modePtr;
			Library.memory.writeInt(mode);
		}
		
		public function get bOnOff():Boolean
		{
			return this._bOnOff;
		}
		
		public function set bOnOff(bOnOff:Boolean):void
		{
			//if (!checkInitialized()) return;
			
			this._bOnOff = bOnOff;
			
			Library.memory.position = bOnOffPtr;
			if (bOnOff)
				Library.memory.writeInt(1);
			else
				Library.memory.writeInt(0);
		}
		
		public function get type():int
		{
			return this._type;
		}
		
		public function set type(type:int):void
		{
			//if (!checkInitialized()) return;
			
			this._type = type;
			
			Library.memory.position = typePtr;
			Library.memory.writeInt(type);
		}
		
		public function get ambient():ColorTransform
		{
			return this._ambient;
		}
		
		public function set ambient(c:ColorTransform):void
		{
			//if (!checkInitialized()) return;
			
			_ambient = c;
			Library.memory.position = ambientPtr;
			Library.memory.writeByte(int(c.redMultiplier * 255));
			Library.memory.writeByte(int(c.greenMultiplier * 255));
			Library.memory.writeByte(int(c.blueMultiplier * 255));
			Library.memory.writeByte(int(c.alphaMultiplier * 255));
		}
		
		public function get diffuse():ColorTransform
		{
			return this._diffuse;
		}
		
		public function set diffuse(c:ColorTransform):void
		{
			//if (!checkInitialized()) return;
			
			_diffuse = c;
			Library.memory.position = diffusePtr;
			Library.memory.writeByte(int(c.redMultiplier * 255));
			Library.memory.writeByte(int(c.greenMultiplier * 255));
			Library.memory.writeByte(int(c.blueMultiplier * 255));
			Library.memory.writeByte(int(c.alphaMultiplier * 255));
		}
		
		public function get specular():ColorTransform
		{
			return this._specular;
		}
		
		public function set specular(c:ColorTransform):void
		{
			//if (!checkInitialized()) return;
			
			_specular = c;
			Library.memory.position = specularPtr;
			Library.memory.writeByte(int(c.redMultiplier * 255));
			Library.memory.writeByte(int(c.greenMultiplier * 255));
			Library.memory.writeByte(int(c.blueMultiplier * 255));
			Library.memory.writeByte(int(c.alphaMultiplier * 255));
		}
		
		public function get range():Number
		{
			return this._range;
		}
		
		public function set range(value:Number):void
		{
			//if (!checkInitialized()) return;
			
			this._range = value;
			Library.memory.position = rangePtr;
			Library.memory.writeFloat(value);
		}
		
		public function get falloff():Number
		{
			return this._falloff;
		}
		
		public function set falloff(value:Number):void
		{
			//if (!checkInitialized()) return;
			
			this._falloff = value;
			Library.memory.position = falloffPtr;
			Library.memory.writeFloat(value);
		}
		
		public function get theta():Number
		{
			return this._theta;
		}
		
		public function set theta(value:Number):void
		{
			//if (!checkInitialized()) return;
			
			this._theta = value;
			Library.memory.position = thetaPtr;
			Library.memory.writeFloat(value);
		}
		
		public function get phi():Number
		{
			return this._phi;
		}
		
		public function set phi(value:Number):void
		{
			//if (!checkInitialized()) return;
			
			this._phi = value;
			Library.memory.position = phiPtr;
			Library.memory.writeFloat(value);
		}
		
		public function get attenuation0():Number
		{
			return this._attenuation0;
		}
		
		public function set attenuation0(value:Number):void
		{
			//if (!checkInitialized()) return;
			
			this._attenuation0 = value;
			Library.memory.position = attenuation0Ptr;
			Library.memory.writeFloat(value);
		}
		
		public function get attenuation1():Number
		{
			return this._attenuation1;
		}
		
		public function set attenuation1(value:Number):void
		{
			//if (!checkInitialized()) return;
			
			this._attenuation1 = value;
			Library.memory.position = attenuation1Ptr;
			Library.memory.writeFloat(value);
		}
		
		public function get attenuation2():Number
		{
			return this._attenuation2;
		}
		
		public function set attenuation2(value:Number):void
		{
			//if (!checkInitialized()) return;
			
			this._attenuation2 = value;
			Library.memory.position = attenuation2Ptr;
			Library.memory.writeFloat(value);
		}
		
		public function Light3D(source:Entity)
		{
			_mode = LightType.HIGH_MODE;
			_bOnOff = LightType.LIGHT_OFF;
			
			_ambient = new ColorTransform(0.4, 0.4, 0.4, 1);
			_diffuse = new ColorTransform(1, 1, 1, 1);
			_specular = new ColorTransform(0.6, 0.6, 0.6, 1);
			
			_source = source;
			
			super();
		}
		
		override protected function initialize():void
		{
			var ps:Array = Library.alchemy3DLib.initializeLight( _source.pointer, _type );

			_pointer = ps[0];
			modePtr = ps[1];
			bOnOffPtr = ps[2];
			typePtr = ps[3];
			ambientPtr = ps[4];
			diffusePtr = ps[5];
			specularPtr = ps[6];
			rangePtr = ps[7];
			falloffPtr = ps[8];
			thetaPtr = ps[9];
			phiPtr = ps[10];
			attenuation0Ptr = ps[11];
			attenuation1Ptr = ps[12];
			attenuation2Ptr = ps[13];
			_node           = ps[14];
		}
		
		/*protected function checkInitialized():Boolean
		{
			if (this.pointer == 0)
			{
				Alchemy3DLog.warning("在设置光源属性前必须先初始化！");
				return false;
			}
			
			return true;
		}*/
	}
}