package cn.alchemy3d.render
{
	public class RenderMode
	{
		/**
		 * 不渲染
		 */
		public static const RENDER_NONE:uint												= 0x000000;
		/**
		 * 32位线框模式
		 */
		public static const RENDER_WIREFRAME_TRIANGLE_32:uint								= 0x000001;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + 纹理映射
		 */
		public static const RENDER_TEXTRUED_TRIANGLE_INVZB_32:uint							= 0x000002;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + 双线性纹理
		 */
		public static const RENDER_TEXTRUED_BILERP_TRIANGLE_INVZB_32:uint					= 0x000004;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + Flat光照模型 + 纹理映射
		 */
		public static const RENDER_TEXTRUED_TRIANGLE_FSINVZB_32:uint						= 0x000008;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + Gouraud光照模型 + 纹理映射
		 */
		public static const RENDER_TEXTRUED_TRIANGLE_GSINVZB_32:uint						= 0x000010;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + 透视矫正纹理映射
		 */
		public static const RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32:uint				= 0x000020;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + 线性分段透视矫正纹理映射
		 */
		public static const RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_INVZB_32:uint			= 0x000040;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + 透视矫正纹理映射 + Flat光照模型
		 */
		public static const RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FSINVZB_32:uint			= 0x000080;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + 线性分段透视矫正纹理映射 + Flat光照模型
		 */
		public static const RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_FSINVZB_32:uint			= 0x000100;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + 透视矫正纹理映射 + Gouraud光照模型
		 */
		public static const RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_GSINVZB_32:uint			= 0x002000;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + Gouraud光照模型 + 纹理映射 + 雾化效果
		 */
		//public static const RENDER_TEXTRUED_TRIANGLE_FOG_GSINVZB_32:uint					= 0x004000;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + 透视矫正纹理映射 + Gouraud光照模型 + 雾化效果
		 */
		public static const RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FOG_GSINVZB_32:uint		= 0x008000;
		
		/**
		 * 32位Flat模式不带Z缓冲
		 */
		//public static const RENDER_FLAT_TRIANGLE_32:uint									= 0x000200;
		
		/**
		 * 32位Flat定点模式不带Z缓冲
		 */
		//public static const RENDER_FLAT_TRIANGLEFP_32:uint									= 0x000400;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + Flat光照模型
		 */
		public static const RENDER_FLAT_TRIANGLE_INVZB_32:uint								= 0x000800;
		
		/**
		 * 32位模式 + 1/z深度缓冲 + Gouraud光照模型
		 */
		public static const RENDER_GOURAUD_TRIANGLE_INVZB_32:uint							= 0x001000;
		
		/**
		 * 
		 */
		public static const RENDER_TEXTRUED_TRIANGLE_GSINVZB_ALPHA_32:uint					= 0x010000;
	}
}