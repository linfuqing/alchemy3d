package cn.alchemy3d.materials
{

	import cn.alchemy3d.events.LoadEvent;
	
	import flash.display.BitmapData;
	import flash.events.EventDispatcher;
	import flash.utils.ByteArray;
	import flash.utils.Endian;

	/**
	* 材质管理器
	* <p/>
	* 每个材质管理器拥有自身的材质列表
	*/
	public class MaterialsManager extends EventDispatcher
	{
		public function MaterialsManager()
		{
			this._materials = new Vector.<AbstractMaterial>;
			this._materialsNum  = this.index = 0;
			this.ready = false;
		}
		
		public var ready:Boolean;
		private var _materials:Vector.<AbstractMaterial>;
		private var _materialsNum:int;
		private var _materialsReadyNum:int;
		private var index:int
		
		public function get materials():Vector.<AbstractMaterial>
		{
			return _materials;
		}

		public function get materialsNum():int
		{
			return _materialsNum;
		}
		
		public function addMaterial(material:AbstractMaterial, name:String=null):void
		{
			if (_materials.indexOf(material) == -1)
			{
				material.name = name || material.name;
				material.ID = index;
				_materialsNum ++;
				index ++;
				_materials.push(material);
				
				if (material.ready)
				{
					if (++ _materialsReadyNum == _materialsNum)
						ready = true;
				}
				else
				{
					ready = false;
					
					material.addEventListener(LoadEvent.TEXTURE_LOAD_COMPLETE, textureLoadCompleteHandler);
				}
			}
		}
		
		/**
		 * 序列化
		 */
		public function serialize():ByteArray
		{
			var buffer:ByteArray = new ByteArray();
			
			if (!ready)
				return buffer;
			
			buffer.endian = Endian.LITTLE_ENDIAN;
			
			var i:int = 0, m:AbstractMaterial, headerSize:uint = 10, pointer:uint = 0;
			var batmapDataByteArray:ByteArray;
			var size:int;
			
			pointer = _materialsNum;
			
			for (; i < _materialsNum; i ++)
			{
				m = _materials[i];
				
				buffer.writeInt(pointer);
				
				pointer += headerSize + m.size;
			}
			
			var bmd:BitmapData;
			var bWidth:int = 0, bHeight:int = 0;
			for (i = 0; i < _materialsNum; i ++)
			{
				m = _materials[i];
				if (m.type == 2)
				{
					bmd = m.bitmapData;
					bWidth = bmd.width;
					bHeight = bmd.height;
				}
				
				//header
				buffer.writeInt(m.type);	//1
				buffer.writeInt(m.alpha);	//2
				buffer.writeInt(bWidth);	//3
				buffer.writeInt(bHeight);	//4
				buffer.writeInt(0);		//5
				buffer.writeInt(0);		//6
				buffer.writeInt(0);		//7
				buffer.writeInt(0);		//8
				buffer.writeInt(0);		//9
				buffer.writeInt(0);		//10
				
				//data
				if (m.type == 2)
				{
					batmapDataByteArray = bmd.getPixels(bmd.rect);
					size = bmd.width * bmd.height;
					batmapDataByteArray.position = 0;
					for (var b:int = 0; b < size; b ++)
					{
						buffer.writeUnsignedInt(batmapDataByteArray.readUnsignedInt());
					}
					//buffer.writeBytes(batmapDataByteArray, 0, batmapDataByteArray.length);
				}
				else
				{
					//buffer.writeInt(Utils.rgbToARGB(m.color, m.alpha));
					buffer.writeInt(m.color);
				}
			}
			
			return buffer;
		}
		
		/**
		* 返回指定ID的材质实例
		* 
		* @param id 材质ID
		* @return 返回材质实例
		*/
		public function getMaterialAt(index:int):AbstractMaterial
		{
			if (index < _materialsNum)
				return this._materials[index];
			else
				return null;
		}
	
		/**
		* 返回指定名称的材质实例
		* 
		* @param name 材质名称
		* @return 返回材质实例
		*/
		public function getMaterialByName(name:String):AbstractMaterial
		{
			for(var i:int = 0; i < _materialsNum; i ++)
			{
				if (name == _materials[i].name)
					return _materials[i];
			}
			return null;
		}
		
		public function getMaterialIDByName(name:String):int
		{
			for(var i:int = 0; i < _materialsNum; i ++)
			{
				if (name == _materials[i].name)
					return i;
			}
			return -1;
		}
	
		/**
		* 删除指定材质实例
		* 
		* @param name 材质实例
		*/
		public function removeMaterial(material:AbstractMaterial):AbstractMaterial
		{
			var i:int = _materials.indexOf(material);
			if(i != -1)
			{
				_materialsNum --;
				return _materials.splice(i, 1)[0];
			}
			else
				return null;
		}
	
		/**
		* 从指定索引位置删除材质实例
		*
		* @param material 索引值
		* @return 返回被删除的材质实例
		*/
		public function removeMaterialAt(index:uint):AbstractMaterial
		{
			if (index < _materialsNum)
			{
				_materialsNum --;
				return this._materials.splice(index, 1)[0];
			}
			else
				return null;
		}
	
		/**
		* 删除指定名称的材质实例
		* 
		* @param name 材质名称
		* @return 返回被删除的材质实例
		*/
		public function removeMaterialByName(name:String):AbstractMaterial
		{
			for(var i:int = 0; i < _materialsNum; i ++)
			{
				if (name == _materials[i].name)
				{
					_materialsNum --;
					return removeMaterialAt(i);
				}
			}
			return null;
		}
	
		/**
		* 以字符串形式返回管理器内所有材质名称
		*
		* @return 返回字符串
		*/
		override public function toString():String
		{
			var list:String = "";
	
			var m:AbstractMaterial;
			for each(m in this._materials)
				list += this._materials[ m ] + "\n";
	
			return list;
		}
		
		private function textureLoadCompleteHandler(e:LoadEvent):void
		{
			e.target.removeEventListener(LoadEvent.TEXTURE_LOAD_COMPLETE, textureLoadCompleteHandler);
			
			if (++ _materialsReadyNum == _materialsNum)
			{
				ready = true;
			}
		}

	}
}