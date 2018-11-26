/*
 * Copy From tp2854sub.c sensor camera driver
 *
 * Copyright (C) 2015-2017 Cogent Embedded, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */
 
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/videodev2.h>

#include <media/soc_camera.h>
#include <media/v4l2-common.h>
#include <media/v4l2-ctrls.h>
#include <linux/of_graph.h>
#include <linux/fs.h> 
#include <linux/file.h> 
#include <linux/mm.h> 
#include <asm/segment.h> 
#include <asm/uaccess.h>
//#include "tp2854.h"

#define DEBUG

#define CHICONY_SUBCAM_0_FAKE_SLAV 0x70
#define CHICONY_SUBCAM_1_FAKE_SLAV 0x71
#define CHICONY_SUBCAM_2_FAKE_SLAV 0x72
#define CHICONY_SUBCAM_3_FAKE_SLAV 0x73

#define NPXL_720P_60 0
#define NPXL_720P_50 1
#define NPXL_720P_30 2
#define NPXL_720P_25 3
#define NPXL_1080P_30 4
#define NPXL_1080P_25 5
#define NPXL_480I 6
#define NPXL_576I 7


struct chiconycam_priv {
	struct v4l2_subdev sd;
	struct v4l2_ctrl_handler hdl;
	struct media_pad pad;
	struct v4l2_rect rect;
	struct v4l2_rect crop_rect;
	int fps_denominator;
	int init_complete;
    int video_mode;
	u8 id[6];
};

static unsigned char chiconycam_id_check = 0;

static inline struct chiconycam_priv *to_chiconycam(const struct i2c_client *client)
{
	return container_of(i2c_get_clientdata(client), struct chiconycam_priv, sd);
}

static int chiconycam_read_reg(struct i2c_client *client ,unsigned char reg  , unsigned char *val)
{
	int ret;
	int tmpaddr ;

    return 0;

//	tmpaddr = client->addr;
//	client->addr = TP2854_ADDR;
//
//	ret = i2c_smbus_read_byte_data(client, reg);
//	if (ret < 0) {
//		dev_err(&client->dev,
//			"read fail: chip 0x%x register 0x%x: %d\n",
//			client->addr, reg, ret);
//	} else {
//		*val = ret;
//	}
// 	client->addr = tmpaddr;
//	return ret < 0 ? ret : 0;
}


static int chiconycam_write_reg(struct i2c_client *client ,unsigned char reg, unsigned char val)
{
	int ret;
	int tmpaddr ;

    return 0;

//	tmpaddr = client->addr;
//	client->addr = TP2854_ADDR; 
//	ret = i2c_smbus_write_byte_data(client, reg, val);
//	if (ret < 0) {
//		dev_err(&client->dev,
//			"%s:write reg error:reg=%2x,val=%2x\n", __func__,
//			reg, val);
//		client->addr = tmpaddr;
//		return -1;
//	}
//	client->addr = tmpaddr;
//	return 0;
}


static int chiconycam_g_mbus_config(struct v4l2_subdev *sd,
				 struct v4l2_mbus_config *cfg)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	pr_debug("chiconycam_g_mbus_config addr %x\n",client->addr);

//	if (client->addr == CHICONY_SUBCAM_0_FAKE_SLAV) {
//         /* one lane , channel 0 */
//        cfg->flags = V4L2_MBUS_CSI2_1_LANE | V4L2_MBUS_CSI2_CHANNEL_0 |
//        V4L2_MBUS_CSI2_CONTINUOUS_CLOCK;
//	} else if (client->addr == CHICONY_SUBCAM_1_FAKE_SLAV) {
//        /* one lane , channel 1 */
//        cfg->flags = V4L2_MBUS_CSI2_1_LANE | V4L2_MBUS_CSI2_CHANNEL_1 |
//        V4L2_MBUS_CSI2_CONTINUOUS_CLOCK;
//	} else if (client->addr == CHICONY_SUBCAM_2_FAKE_SLAV) {
//        /* one lane , channel 2 */
//        cfg->flags = V4L2_MBUS_CSI2_1_LANE | V4L2_MBUS_CSI2_CHANNEL_2 |
//        V4L2_MBUS_CSI2_CONTINUOUS_CLOCK;
//	} else if (client->addr == CHICONY_SUBCAM_3_FAKE_SLAV) {
//        /* one lane , channel 3 */
//        cfg->flags = V4L2_MBUS_CSI2_1_LANE | V4L2_MBUS_CSI2_CHANNEL_3 |
//        V4L2_MBUS_CSI2_CONTINUOUS_CLOCK;
//	}

    cfg->flags = V4L2_MBUS_CSI2_1_LANE | V4L2_MBUS_CSI2_CHANNEL_0 |
    V4L2_MBUS_CSI2_CONTINUOUS_CLOCK;
	cfg->type = V4L2_MBUS_CSI2;
	return 0;
}


static int chiconycam_s_stream(struct v4l2_subdev *sd, int enable)
{
//	struct i2c_client *client = v4l2_get_subdevdata(sd);

	pr_debug("chiconycam_s_stream\n");
	return 0;
}

static int chiconycam_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
	/* struct i2c_client *client = v4l2_get_subdevdata(sd); */
	struct v4l2_captureparm *cp = &parms->parm.capture;

	pr_debug("chiconycam_s_parm\n");

    //if not video capture , return error
	if (parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	if (cp->extendedmode != 0)
		return -EINVAL;

	return 0;
}

static int chiconycam_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct v4l2_captureparm *cp = &parms->parm.capture;
    struct chiconycam_priv *priv = i2c_get_clientdata(client);

	pr_debug("chiconycam_g_parm\n");

	if (parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	memset(cp, 0, sizeof(struct v4l2_captureparm));

	//cp->capability = V4L2_CAP_TIMEPERFRAME; not support

    /*
     * timeperframe.discrete.denominator / timeperframe.discrete.numerator
     */

	cp->timeperframe.numerator = 1;
	cp->timeperframe.denominator = priv->fps_denominator;

	return 0;
}


static int chiconycam_get_fmt(struct v4l2_subdev *sd,
			   struct v4l2_subdev_pad_config *cfg,
			   struct v4l2_subdev_format *format)
{
	struct v4l2_mbus_framefmt *mf = &format->format;
	struct i2c_client *client = v4l2_get_subdevdata(sd);
    struct chiconycam_priv *priv = i2c_get_clientdata(client);

	pr_debug("chiconycam_get_fmt\n");

	if (format->pad)
		return -EINVAL;

	mf->width = priv->rect.width;
	mf->height = priv->rect.height;
	mf->code = MEDIA_BUS_FMT_YUYV8_2X8; 
	mf->colorspace = V4L2_COLORSPACE_SMPTE170M;

    if (priv->video_mode == NPXL_480I || priv->video_mode == NPXL_576I) {
        /* interlaced mode */
        mf->field = V4L2_FIELD_INTERLACED;
    } else {
        /* Progressive mode */
        mf->field = V4L2_FIELD_NONE;
    }

	return 0;
}

static int chiconycam_set_fmt(struct v4l2_subdev *sd,
			   struct v4l2_subdev_pad_config *cfg,
			   struct v4l2_subdev_format *format)
{
	struct v4l2_mbus_framefmt *mf = &format->format;
	/* struct i2c_client *client = v4l2_get_subdevdata(sd); */

	pr_debug("chiconycam_set_fmt\n");

	mf->code = MEDIA_BUS_FMT_YUYV8_2X8;
	mf->colorspace = V4L2_COLORSPACE_SMPTE170M;
	mf->field = V4L2_FIELD_NONE;
	
	if (format->which == V4L2_SUBDEV_FORMAT_TRY)
		cfg->try_fmt = *mf;

	return 0;
}

static int chiconycam_get_selection(struct v4l2_subdev *sd,
    struct v4l2_subdev_pad_config *cfg, struct v4l2_subdev_selection *sel)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct chiconycam_priv *priv = to_chiconycam(client);

	pr_debug("chiconycam_get_selection\n");
    
	if (sel->which != V4L2_SUBDEV_FORMAT_ACTIVE)
		return -EINVAL;

	switch (sel->target) {
	case V4L2_SEL_TGT_CROP_BOUNDS:
        pr_debug("V4L2_SEL_TGT_CROP_BOUNDS\r\n");
		sel->r.left = 0;
		sel->r.top = 0;
		sel->r.width = priv->crop_rect.width;
		sel->r.height = priv->crop_rect.height;
		return 0;

	case V4L2_SEL_TGT_CROP_DEFAULT:
        pr_debug("V4L2_SEL_TGT_CROP_DEFAULT\r\n");
		sel->r.left = 0;
		sel->r.top = 0;
		sel->r.width = priv->crop_rect.width;
		sel->r.height = priv->crop_rect.height;
		return 0;

	case V4L2_SEL_TGT_CROP:
        pr_debug("V4L2_SEL_TGT_CROP\r\n");
		sel->r = priv->crop_rect;
		return 0;

	default:
		return -EINVAL;
	}
}


static int chiconycam_set_selection(struct v4l2_subdev *sd,
    struct v4l2_subdev_pad_config *cfg, struct v4l2_subdev_selection *sel)
{
	struct v4l2_rect *rect = &sel->r;
	struct i2c_client *client = v4l2_get_subdevdata(sd);
    struct chiconycam_priv *priv = to_chiconycam(client);

	pr_debug("chiconycam_set_selection\n");

	if (sel->which != V4L2_SUBDEV_FORMAT_ACTIVE ||
	    sel->target != V4L2_SEL_TGT_CROP)
		return -EINVAL;

//	rect->left = ALIGN(rect->left, 2);
//	rect->top = ALIGN(rect->top, 2);
//	rect->width = ALIGN(rect->width, 2);
//	rect->height = ALIGN(rect->height, 2);

    pr_debug("rect->left %d\r\n", rect->left);
    pr_debug("rect->width %d\r\n", rect->width);
    pr_debug("rect->top %d\r\n", rect->top);
    pr_debug("rect->height %d\r\n", rect->height);

	if ((rect->left + rect->width > priv->rect.width) ||
	    (rect->top + rect->height > priv->rect.height))
		*rect = priv->rect;

//	rect->left = 0;
//	rect->top = 0;
//	rect->width = priv->rect.width;
//	rect->height = priv->rect.height;

	priv->crop_rect.left = rect->left;
	priv->crop_rect.top = rect->top;
	priv->crop_rect.width = rect->width;
	priv->crop_rect.height = rect->height;

    return 0;
}


static int chiconycam_enum_mbus_code(struct v4l2_subdev *sd,
    struct v4l2_subdev_pad_config *cfg, struct v4l2_subdev_mbus_code_enum *code)
{
	/* struct i2c_client *client = v4l2_get_subdevdata(sd); */
	if (code->pad || code->index > 0)
		return -EINVAL;

	/* pr_debug("chiconycam_enum_mbus_code\n"); */
//	code->code = MEDIA_BUS_FMT_YUYV10_2X10;
	code->code = MEDIA_BUS_FMT_YUYV8_2X8;
//	code->code = MEDIA_BUS_FMT_RGB888_1X24;
//	code->code = MEDIA_BUS_FMT_SBGGR12_1X12;
	return 0;
}


static int chiconycam_s_power(struct v4l2_subdev *sd, int on)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);

    pr_info("%s on = %d\n", __func__, on);

//    tp2854_write_reg(client ,TP2854_PAGE, MIPI_PAGE_ENABLE);
	if (on) {
//		tp2854_write_reg(client ,TP2854_MIPI_STOPCLK, MIPI_CLK_NORMAL);
//		tp2854_write_reg(client ,TP2854_MIPI_OUTPUT_EN, ENABLE_ALL_LANES);
	} else {
//        tp2854_write_reg(client ,TP2854_MIPI_STOPCLK, MIPI_CLK_STOP);
//		tp2854_write_reg(client ,TP2854_MIPI_OUTPUT_EN, DISABLE_ALL_LANES);
	}
    /* msleep(50); */
	return 0;
}

#ifdef CONFIG_VIDEO_TVI_DEBUG
static int chiconycam_g_register(struct v4l2_subdev *sd,
				      struct v4l2_dbg_register *reg)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret = 0;
	u8 val = 0;

//	ret = tp2854_read_reg(client, (u8)reg->reg, &val);
//	if (ret < 0)
//		return ret;
//
//	reg->val = val;
//	reg->size = sizeof(u8);

	return 0;
}

static int chiconycam_s_register(struct v4l2_subdev *sd,
				      const struct v4l2_dbg_register *reg)
{
	/* struct i2c_client *client = v4l2_get_subdevdata(sd); */

	return 0;
}
#endif

static struct v4l2_subdev_core_ops chiconycam_core_ops = {
#ifdef CONFIG_VIDEO_TVP_DEBUG
	.g_register = chiconycam_g_register,
	.s_register = chiconycam_s_register,
#endif
	.s_power = chiconycam_s_power,
};



static const struct v4l2_subdev_pad_ops chiconycam_subdev_pad_ops = {
	.enum_mbus_code = chiconycam_enum_mbus_code,
	.set_selection = chiconycam_set_selection,
	.get_selection = chiconycam_get_selection,
	.set_fmt = chiconycam_set_fmt,
	.get_fmt = chiconycam_get_fmt,
};

static const struct v4l2_subdev_video_ops chiconycam_video_ops = {
	.s_stream = chiconycam_s_stream,
	.g_mbus_config = chiconycam_g_mbus_config,
	.g_parm = chiconycam_g_parm,
	.s_parm = chiconycam_s_parm,
};

static struct v4l2_subdev_ops chiconycam_subdev_ops = {
	.video = &chiconycam_video_ops,
	.pad = &chiconycam_subdev_pad_ops,
	.core = &chiconycam_core_ops,
};

static const struct i2c_device_id chiconycam_id[] = {
	{ "chiconycam", 0 },
	{ }
};

int chiconycam_check_deviceId(struct i2c_client *client)
{
	u8 val = 0; 

    pr_debug("%s\r\n", __func__);

//    tp2854_write_reg(client ,TP2854_PAGE, MIPI_PAGE_DISABLE);
//
//	/* read TP2854_ID_1 */
//	tp2854_read_reg(client, TP2854_ID1_REG, &val);
//	pr_debug("tp2854 read 0xfe %x\n", val);
//	if (val != TP2854_ID_1) {
//		pr_err("dismatch tp2854 device Id 1");
//		return -1;
//	}
//
//    /* read TP2854_ID_2 */
//	tp2854_read_reg(client, TP2854_ID2_REG, &val);
//	pr_debug("tp2854 read 0xff %x\n", val);
//	if (val != TP2854_ID_2) {
//		pr_err("dismatch tp2854 device Id 1");
//		return -1;
//	}

    return 1;
}


static int chiconycam_parse_dt(struct i2c_client *client)
{
    struct device_node *np = client->dev.of_node;
    struct device_node *endpoint = NULL, *rendpoint = NULL;
    struct device_node *p_endpoint = NULL, *np_endpoint = NULL;
    int i, nxpl = 0;
    struct chiconycam_priv *priv = i2c_get_clientdata(client);

    for (i = 0; ; i++) {
        endpoint = of_graph_get_next_endpoint(np, endpoint);
        if (!endpoint)
            break;

        rendpoint = of_parse_phandle(endpoint, "remote-endpoint", 0);
        if (rendpoint) {
            p_endpoint = of_get_next_parent(rendpoint);
            if (!p_endpoint)
                continue;

            np_endpoint = of_get_next_parent(p_endpoint);
            if (!np_endpoint) 
                continue;

            if (!of_property_read_u32(np_endpoint, "npxl", &nxpl)) {
                priv->video_mode = nxpl;
                return 0;
            }
        }
    }
    return 0;
}

static void chiconycam_video_mode(struct chiconycam_priv *priv)
{
  switch (priv->video_mode) {
        case NPXL_720P_60:
            pr_debug("%s USE NPXL_720P_60\r\n", __func__);
            priv->rect.left = 0;
            priv->rect.top = 0;
            priv->rect.width = 1280;
            priv->rect.height = 720;
            priv->fps_denominator = 60;
            break;

        case NPXL_720P_50:
            priv->rect.left = 0;
            priv->rect.top = 0;
            priv->rect.width = 1280;
            priv->rect.height = 720;
            priv->fps_denominator = 50;
            break;

        case NPXL_720P_30:
            pr_debug("%s USE NPXL_720P_30\r\n", __func__);
            priv->rect.left = 0;
            priv->rect.top = 0;
            priv->rect.width = 1280;
            priv->rect.height = 720;
            priv->fps_denominator = 30;
            break;

        case NPXL_720P_25:
            priv->rect.left = 0;
            priv->rect.top = 0;
            priv->rect.width = 1280;
            priv->rect.height = 720;
            priv->fps_denominator = 25;
            break;

        case NPXL_1080P_30:
            pr_debug("%s USE NPXL_1080P_30\r\n", __func__);
            priv->rect.left = 0;
            priv->rect.top = 0;
            priv->rect.width = 1920;
            priv->rect.height = 1080;
            priv->fps_denominator = 30;
            break;

        case NPXL_1080P_25:
            pr_debug("%s USE NPXL_1080P_25\r\n", __func__);
            priv->rect.left = 0;
            priv->rect.top = 0;
            priv->rect.width = 1920;
            priv->rect.height = 1080;
            priv->fps_denominator = 25;
            break;

        case NPXL_480I:
            pr_debug("%s USE NPXL_480I\r\n", __func__);
            priv->rect.left = 0;
            priv->rect.top = 0;
            priv->rect.width = 1920;
            priv->rect.height = 240;
            priv->fps_denominator = 50;
            break;

        case NPXL_576I:
            break;

        default:
        break;
    };
    priv->crop_rect = priv->rect;
}


#define DRIVER_VERSION "0.1"

static int chiconycam_probe(struct i2c_client *client,
			 const struct i2c_device_id *did)
{
    struct chiconycam_priv *priv;
	int ret;

	pr_info("%s ver %s\n", __func__, DRIVER_VERSION);
/*
    if (chiconycam_id_check == 0 && chiconycam_check_deviceId(client) < 0) {
        return -1;
    }
*/
    chiconycam_id_check = 1;

	priv = devm_kzalloc(&client->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	//initialize I2C
	v4l2_i2c_subdev_init(&priv->sd, client, &chiconycam_subdev_ops);
	priv->sd.flags = V4L2_SUBDEV_FL_HAS_DEVNODE;


    /* chiconycam_parse_dt(client); */
    
    priv->video_mode = NPXL_720P_30;
    chiconycam_video_mode(priv);
	priv->pad.flags = MEDIA_PAD_FL_SOURCE;
	priv->sd.entity.flags |= MEDIA_ENT_F_CAM_SENSOR;
    priv->sd.host_priv = priv;
	ret = media_entity_pads_init(&priv->sd.entity, 1, &priv->pad);
	if (ret < 0)
		goto cleanup;
	ret = v4l2_async_register_subdev(&priv->sd);
	if (ret)
		goto cleanup;

	priv->init_complete = 1;

	return 0;

cleanup:
	media_entity_cleanup(&priv->sd.entity);
	v4l2_ctrl_handler_free(&priv->hdl);
	v4l2_device_unregister_subdev(&priv->sd);

	return ret;
}


static int chiconycam_remove(struct i2c_client *client)
{
	struct chiconycam_priv *priv = i2c_get_clientdata(client);

	v4l2_async_unregister_subdev(&priv->sd);
	media_entity_cleanup(&priv->sd.entity);
	v4l2_ctrl_handler_free(&priv->hdl);
	v4l2_device_unregister_subdev(&priv->sd);
	return 0;
}


/*
MODULE_DEVICE_TABLE(i2c, chiconycam_id);

static const struct of_device_id chiconycam_of_ids[] = {
	{ .compatible = "tp,tp2864_sub_cam", },
	{ }
};

static struct i2c_driver chiconycam_i2c_driver = {
	.driver	= {
		.name		= "chiconycam",
		.of_match_table	= chiconycam_of_ids,
	},
	.probe		= chiconycam_probe,
	.remove		= chiconycam_remove,
	.id_table	= chiconycam_id,
};

module_i2c_driver(chiconycam_i2c_driver);
*/

MODULE_DESCRIPTION("chicony SUB_CAM Driver");
MODULE_LICENSE("GPL");

