package cn.alchemy3d.render
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;
	import cn.alchemy3d.tools.Alchemy3DLog;
	
	import flash.geom.ColorTransform;
	
	public class Material extends Pointer
	{
		//public var pointer:uint = 0;
		
		private var ambientPtr:uint;
		private var diffusePtr:uint;
		private var specularPtr:uint;
		private var emissivePtr:uint;
		private var powerPtr:uint;
		private var doubleSidePtr:uint;
		
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
		
		public function get emissive():ColorTransform
		{
			return this._emissive;
		}
		
		public function set emissive(c:ColorTransform):void
		{
			//if (!checkInitialized()) return;
			
			_emissive = c;
			Library.memory.position = emissivePtr;
			Library.memory.writeByte(int(c.redMultiplier * 255));
			Library.memory.writeByte(int(c.greenMultiplier * 255));
			Library.memory.writeByte(int(c.blueMultiplier * 255));
			Library.memory.writeByte(int(c.alphaMultiplier * 255));
		}
		
		public function get power():Number
		{
			return this._power;
		}
		
		public function set power(value:Number):void
		{
			//if (!checkInitialized()) return;
			
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
			//if (!checkInitialized()) return;
			
			_doubleSide = bool;
			
			Library.memory.position = doubleSidePtr;
			
			Library.memory.writeFloat( bool ? TRUE : FALSE );
		}
		
		public function Material(ambient:ColorTransform = null, diffuse:ColorTransform = null, specular:ColorTransform = null, emissive:ColorTransform = null, power:Number = 32)
		{
			_ambient = ambient || new ColorTransform(.1, .1, .1, 1);
			_diffuse = diffuse || new ColorTransform(.6, .6, .6, 1);
			_specular = specular || new ColorTransform(.8, .8, .8, 1);
			_emissive = emissive || new ColorTransform(0, 0, 0, 1);
			_power = power;
			
			super();
		}
		
		override protected function initialize():void
		{	
			var ps:Array = Library.alchemy3DLib.initializeMaterial(_ambient, _diffuse, _specular, _emissive, _power);
			_pointer     = ps[0];
			ambientPtr   = ps[1];
			diffusePtr   = ps[2];
			specularPtr  = ps[3];
			emissivePtr  = ps[4];
			powerPtr     = ps[5];
		}
		
		/*protected function checkInitialized():Boolean
		{
			if (this.pointer == 0)
			{
				Alchemy3DLog.warning("在设置材质属性前必须先初始化！");
				return false;
			}
			
			return true;
		}*/
	}
}