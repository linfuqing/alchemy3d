package cn.alchemy3d.materials
{
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.tools.Alchemy3DLog;
	
	import flash.geom.ColorTransform;
	import flash.utils.ByteArray;
	
	public class Material
	{
		public var pointer:uint = 0;
		
		public var ambientPtr:uint;
		public var diffusePtr:uint;
		public var specularPtr:uint;
		public var emissivePtr:uint;
		public var powerPtr:uint;
		public var doubleSidePtr:uint;
		
		private var _ambient:ColorTransform;	//指定此表面反射的环境光数量
		private var _diffuse:ColorTransform;	//指定此表面反射的漫射光数量
		private var _specular:ColorTransform;	//指定此表面反射的镜面光数量
		private var _emissive:ColorTransform;	//这个是被用来给表面添加颜色，它使得物体看起来就象是它自己发出的光一样
		private var _power:Number;				//指定锐利的镜面高光；它的值是高光的锐利值
		private var _doubleSide:Boolean;
		
		public function get ambient():ColorTransform
		{
			return this._ambient;
		}
		
		public function set ambient(c:ColorTransform):void
		{
			if (!checkInitialized()) return;
			
			_ambient = c;
			Library.memory.position = ambientPtr;
			Library.memory.writeFloat(c.redMultiplier);
			Library.memory.writeFloat(c.greenMultiplier);
			Library.memory.writeFloat(c.blueMultiplier);
			Library.memory.writeFloat(c.alphaMultiplier);
		}
		
		public function get diffuse():ColorTransform
		{
			return this._diffuse;
		}
		
		public function set diffuse(c:ColorTransform):void
		{
			if (!checkInitialized()) return;
			
			_diffuse = c;
			Library.memory.position = diffusePtr;
			Library.memory.writeFloat(c.redMultiplier);
			Library.memory.writeFloat(c.greenMultiplier);
			Library.memory.writeFloat(c.blueMultiplier);
			Library.memory.writeFloat(c.alphaMultiplier);
		}
		
		public function get specular():ColorTransform
		{
			return this._specular;
		}
		
		public function set specular(c:ColorTransform):void
		{
			if (!checkInitialized()) return;
			
			_specular = c;
			Library.memory.position = specularPtr;
			Library.memory.writeFloat(c.redMultiplier);
			Library.memory.writeFloat(c.greenMultiplier);
			Library.memory.writeFloat(c.blueMultiplier);
			Library.memory.writeFloat(c.alphaMultiplier);
		}
		
		public function get emissive():ColorTransform
		{
			return this._emissive;
		}
		
		public function set emissive(c:ColorTransform):void
		{
			if (!checkInitialized()) return;
			
			_emissive = c;
			Library.memory.position = emissivePtr;
			Library.memory.writeFloat(c.redMultiplier);
			Library.memory.writeFloat(c.greenMultiplier);
			Library.memory.writeFloat(c.blueMultiplier);
			Library.memory.writeFloat(c.alphaMultiplier);
		}
		
		public function get power():Number
		{
			return this._power;
		}
		
		public function set power(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_power = value;
			Library.memory.position = powerPtr;
			Library.memory.writeFloat(value);
		}
		
		public function get doubleSide():Boolean
		{
			return this._doubleSide;
		}
		
		public function set doubleSide(bool:Boolean):void
		{
			if (!checkInitialized()) return;
			
			_doubleSide = bool;
			Library.memory.position = doubleSidePtr;
			
			if (bool)
				Library.memory.writeFloat(1);
			else
				Library.memory.writeFloat(0);
		}
		
		public function Material()
		{
			_ambient = new ColorTransform(0, 0, 0, 0);
			_diffuse = new ColorTransform(0, 0, 0, 0);
			_specular = new ColorTransform(0, 0, 0, 0);
			_emissive = new ColorTransform(0, 0, 0, 0);
			_power = 4;
			
			var ps:Array = Library.alchemy3DLib.initializeMaterial();
			pointer = ps[0];
			ambientPtr = ps[1];
			diffusePtr = ps[2];
			specularPtr = ps[3];
			emissivePtr = ps[4];
			powerPtr = ps[5];
		}
		
		protected function checkInitialized():Boolean
		{
			if (this.pointer == 0)
			{
				Alchemy3DLog.warning("在设置材质属性前必须先初始化！");
				return false;
			}
			
			return true;
		}
	}
}