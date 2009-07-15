package cn.alchemy3d.lights
{
	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.objects.Entity;
	import cn.alchemy3d.objects.ISceneNode;
	import cn.alchemy3d.tools.Alchemy3DLog;
	
	import flash.geom.ColorTransform;
	import flash.utils.ByteArray;

	public class Light3D implements ISceneNode
	{
		public var pointer:uint = 0;
		protected var buffer:ByteArray;
		protected var lib:Alchemy3DLib;
		
		protected var type:int;			//定义灯光类型，我们能够使用下面三种类型之一：D3DLIGHT_POINT, D3DLIGHT_SPOT, D3DLIGHT_DIRECTIONAL
		
		public var ambientPtr:uint;
		public var diffusePtr:uint;
		public var specularPtr:uint;
		public var rangePtr:uint;
		public var falloffPtr:uint;
		public var thetaPtr:uint;
		public var phiPtr:uint;
		public var attenuation0Ptr:uint;
		public var attenuation1Ptr:uint;
		public var attenuation2Ptr:uint;
		
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
		
		public var source:Entity;
		
		public function get ambient():ColorTransform
		{
			return this._ambient;
		}
		
		public function set ambient(c:ColorTransform):void
		{
			if (!checkInitialized()) return;
			
			_ambient = c;
			buffer.position = ambientPtr;
			buffer.writeFloat(c.redMultiplier);
			buffer.writeFloat(c.greenMultiplier);
			buffer.writeFloat(c.blueMultiplier);
			buffer.writeFloat(c.alphaMultiplier);
		}
		
		public function get diffuse():ColorTransform
		{
			return this._diffuse;
		}
		
		public function set diffuse(c:ColorTransform):void
		{
			if (!checkInitialized()) return;
			
			_diffuse = c;
			buffer.position = diffusePtr;
			buffer.writeFloat(c.redMultiplier);
			buffer.writeFloat(c.greenMultiplier);
			buffer.writeFloat(c.blueMultiplier);
			buffer.writeFloat(c.alphaMultiplier);
		}
		
		public function get specular():ColorTransform
		{
			return this._specular;
		}
		
		public function set specular(c:ColorTransform):void
		{
			if (!checkInitialized()) return;
			
			_specular = c;
			buffer.position = specularPtr;
			buffer.writeFloat(c.redMultiplier);
			buffer.writeFloat(c.greenMultiplier);
			buffer.writeFloat(c.blueMultiplier);
			buffer.writeFloat(c.alphaMultiplier);
		}
		
		public function get range():Number
		{
			return this._range;
		}
		
		public function set range(value:Number):void
		{
			if (!checkInitialized()) return;
			
			this._range = value;
			buffer.position = rangePtr;
			buffer.writeFloat(value);
		}
		
		public function get falloff():Number
		{
			return this._falloff;
		}
		
		public function set falloff(value:Number):void
		{
			if (!checkInitialized()) return;
			
			this._falloff = value;
			buffer.position = falloffPtr;
			buffer.writeFloat(value);
		}
		
		public function get theta():Number
		{
			return this._theta;
		}
		
		public function set theta(value:Number):void
		{
			if (!checkInitialized()) return;
			
			this._theta = value;
			buffer.position = thetaPtr;
			buffer.writeFloat(value);
		}
		
		public function get phi():Number
		{
			return this._phi;
		}
		
		public function set phi(value:Number):void
		{
			if (!checkInitialized()) return;
			
			this._phi = value;
			buffer.position = phiPtr;
			buffer.writeFloat(value);
		}
		
		public function get attenuation0():Number
		{
			return this._attenuation0;
		}
		
		public function set attenuation0(value:Number):void
		{
			if (!checkInitialized()) return;
			
			this._attenuation0 = value;
			buffer.position = attenuation0Ptr;
			buffer.writeFloat(value);
		}
		
		public function get attenuation1():Number
		{
			return this._attenuation1;
		}
		
		public function set attenuation1(value:Number):void
		{
			if (!checkInitialized()) return;
			
			this._attenuation1 = value;
			buffer.position = attenuation1Ptr;
			buffer.writeFloat(value);
		}
		
		public function get attenuation2():Number
		{
			return this._attenuation2;
		}
		
		public function set attenuation2(value:Number):void
		{
			if (!checkInitialized()) return;
			
			this._attenuation2 = value;
			buffer.position = attenuation2Ptr;
			buffer.writeFloat(value);
		}
		
		public function Light3D()
		{
			_ambient = new ColorTransform(0.4, 0.4, 0.4, 1);
			_diffuse = new ColorTransform(1, 1, 1, 1);
			_specular = new ColorTransform(0.6, 0.6, 0.6, 1);
			
			lib = Alchemy3DLib.getInstance();
			buffer = lib.buffer;
			
			this.source = source == null ? new Entity(null, "lightSource") : source;
		}
		
		public function initialize(scenePtr:uint, parentPtr:uint):void
		{
			source.allotPtr(lib.alchemy3DLib.initializeEntity(0, 0, 0, 0, 0, 0));
			
			allotPtr(lib.alchemy3DLib.initializeLight(scenePtr, source.pointer, type));
		}
		
		public function allotPtr(ps:Array):void
		{
			pointer = ps[0];
			ambientPtr = ps[1];
			diffusePtr = ps[2];
			specularPtr = ps[3];
			rangePtr = ps[4];
			falloffPtr = ps[5];
			thetaPtr = ps[6];
			phiPtr = ps[7];
			attenuation0Ptr = ps[8];
			attenuation1Ptr = ps[9];
			attenuation2Ptr = ps[10];
		}
		
		protected function checkInitialized():Boolean
		{
			if (this.pointer == 0)
			{
				Alchemy3DLog.warning("在设置光源属性前必须先初始化！");
				return false;
			}
			
			return true;
		}
	}
}