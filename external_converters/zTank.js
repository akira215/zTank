const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const log = require('zigbee-herdsman-converters/lib/logger');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const utils = require('zigbee-herdsman-converters/lib/utils');
const globalStore = require('zigbee-herdsman-converters/lib/store');

// settings from zigbee2mqtt to access changeEntityOptions
const settings = require('/app/dist/util/settings');

const logger = log.logger;
const e = exposes.presets;
const ea = exposes.access;

// Global /////////////////////////

const WATERMETER_EP = 1;
const UPSTREAM_EP   = 2;
const DOWNSTREAM_EP = 3;

// GUI elements ///////////////////////////////////////////////////////////////////////////

function genZtank() {
    // The name shall not contain space otherwise it is not reported to HA
    return [
            exposes.numeric('water_consumed', ea.STATE_GET)
                .withDescription('Water meter record, reset to 0 after each reading')
                .withUnit('L'),
                //.withEndoint('Name'),
            exposes.numeric('upstream_pressure', ea.STATE_GET)
                .withDescription('Pressure on pump side')
                .withUnit('bars'),
            exposes.numeric('downstream_pressure', ea.STATE_GET)
                .withDescription('Pressure on network side')
                .withUnit('bars'),
            exposes.numeric('delta_pressure', ea.STATE_GET)
                .withDescription('Delta Pressure between filters')
                .withUnit('mbars'),
        ];
};

// Options ///////////////////////////////////////////////////////////////////////////

// Options for the k factor of the water meter (L/pule)
function genWaterMeterOptions() {
    return  exposes.numeric('k_factor', ea.ALL)
                .withValueMin(0)
                .withDescription('Water meter kFactor')
                .withUnit('L/pulse')
};

function genUpstreamPressureOptions() {
    return  exposes.numeric('upstream_pressure_calibration', ea.ALL)
                .withValueMin(0)
                .withDescription('Sensor calibrating factor for upstream pressure (Pascal per Volt)')
                .withUnit('Pa/V')
};

function genDownstreamPressureOptions() {
    return  exposes.numeric('downstream_pressure_calibration', ea.ALL)
                .withValueMin(0)
                .withDescription('Sensor calibrating factor for downstream pressure (Pascal per Volt)')
                .withUnit('Pa/V')
};

// toZigbee ///////////////////////////////////////////////////////////////////////////

const toZigbee_Metering = {
    //key: ['Water Consumption', 'switch_actions', 'relay_mode', 'max_pause', 'min_long_press'],
    key: ['water_consumed'],
    convertSet: async (entity, key, value, meta) => {
        // just for debug, to see any incoming request
        logger.info(` convertSet key=[${key}]`);
    },
    // convertGet will be call to read value on the device
    convertGet: async (entity, key, meta) => {
        const endpoint = meta.device.getEndpoint(WATERMETER_EP);
        // If Water Consumption actualization button has been pressed
        if(key == 'water_consumed') {
            logger.info(` ---- Reading ----- Ep${JSON.stringify(endpoint)}`);
            await endpoint.read('msFlowMeasurement', ['measuredValue'/*,'unitOfMeasure','multiplier','divisor','summaFormatting'*/]);
        }
    },

};

const toZigbee_Pressure = {
    key: ['upstream_pressure', 'downstream_pressure'],
    convertSet: async (entity, key, value, meta) => {
        // just for debug, to see any incoming request
        logger.info(` convertSet key=[${key}]`);
        //logger.info(` convertSet key=[${JSON.stringify(entity)}]`);
    },
    // convertGet will be call to read value on the device
    convertGet: async (entity, key, meta) => {
        //logger.info(` convertGet entity=[${JSON.stringify(entity)}]`);
        //logger.info(` convertSet key=[${JSON.stringify(key)}]`);
        //logger.info(` convertSet meta=[${JSON.stringify(meta)}]`);
        const endpointUpstream      = meta.device.getEndpoint(UPSTREAM_EP);
        const endpointDownstream    = meta.device.getEndpoint(DOWNSTREAM_EP);

        // If Upstream Pressure actualization button has been pressed
        if(key == 'upstream_pressure') {
            await endpointUpstream.read('msPressureMeasurement', ['measuredValue']);
        }

        if(key == 'downstream_pressure') {
            await endpointDownstream.read('msPressureMeasurement', ['measuredValue']);
        }
    },

};

// fromZigbee ///////////////////////////////////////////////////////////////////////////

const fromZigbee_Metering = {
    
    cluster: 'msFlowMeasurement',
    type: ['attributeReport', 'readResponse'],
    options: [genWaterMeterOptions()],
    convert: (model, msg, publish, options, meta) => {
        const result = {};
        //meta.logger.info(`convert meta =${JSON.stringify(meta)}`);
        //meta.logger.info(`convert publish =${JSON.stringify(publish)}`);
        //meta.logger.info(`convert msg =${JSON.stringify(msg)}`);


        if (msg.data.hasOwnProperty('measuredValue')) {
            const multiplier = options.k_factor;
            //const divisor = meta.device.getEndpoint(1).getClusterAttributeValue('seMetering', 'divisor');
            //meta.logger.info(`+_+_+_ #2 Multiplier=${multiplier}`);
            //meta.logger.info(`+_+_+_ #3 options=${JSON.stringify(options)}`);
            const data = msg.data['measuredValue'];
            //Needed BigInt as << 32 overpass the Number JS type
            //const currentSummDelivered = (BigInt(parseInt(data[0])) << BigInt(32)) + BigInt(parseInt(data[1]));
            
            //meta.logger.info(`+_+_+_ Datas=${data}`)
            
            //Multiply by the divisor and divide again to avoid BigInt rounding to 0
            const currentSummDisplayed = (Number(data) * multiplier);
            
            result[`water_consumed`] = currentSummDisplayed;
        }

        return result;
    },
}

const fromZigbee_Pressure = {
    
    cluster: 'msPressureMeasurement',
    type: ['attributeReport', 'readResponse'],
    options: [ genUpstreamPressureOptions(), genDownstreamPressureOptions() ],
    convert: (model, msg, publish, options, meta) => {
        const result = {};
        //logger.info(`convert msg =${JSON.stringify(msg)}`);


        if (msg.data.hasOwnProperty('measuredValue')) {

            if(msg.endpoint.ID == UPSTREAM_EP)
            {
                const value = msg.data['measuredValue'];
                //const multiplier = options.k_factor;
                logger.info(`+_+_+_ fromZigbee received for UPSTREAM value ${value}`);
                result[`upstream_pressure`] = value;
            }

            if(msg.endpoint.ID == DOWNSTREAM_EP)
            {
                const value = msg.data['measuredValue'];
                logger.info(`+_+_+_ fromZigbee received for DOWNSTREAM value ${value}`);
                result[`downstream_pressure`] = value;
            }
            const multiplier = options.k_factor;
            //const divisor = meta.device.getEndpoint(1).getClusterAttributeValue('seMetering', 'divisor');
            //meta.logger.info(`+_+_+_ #2 Multiplier=${multiplier}`);
            
            const data = msg.data['measuredValue'];
            //Needed BigInt as << 32 overpass the Number JS type
            //const currentSummDelivered = (BigInt(parseInt(data[0])) << BigInt(32)) + BigInt(parseInt(data[1]));

            
            //Multiply by the divisor and divide again to avoid BigInt rounding to 0
            const currentSummDisplayed = (Number(data) * multiplier);
            
            //result[`upstream_pressure`] = currentSummDisplayed;

            /////////////////////////
            ////////////////////////
            // UPDATE deltaP
            ///////////////////////
            ///////////////////////
        }

        return result;
    },
}


// This is triggered after device pairing (if loosing connection)
// Factors are saved on the device itself and read by z2m to update the values
const fromZigbee_kFactor = {
    
    cluster: 'genAnalogValue',
    type: ['attributeReport', 'readResponse'],
    options: [genWaterMeterOptions()],
    convert: (model, msg, publish, options, meta) => {
        const result = {};
        logger.info(`fromZigbee_kFactor convert msg =${JSON.stringify(msg)}`);


        //Multiplier is read only on configure device, we use this to set up the optional setting from the state
        if (msg.data.hasOwnProperty('presentValue')) {
            //logger.info(`+-+-+-+-+-+-+-+-+-+-+-+- msg data =${JSON.stringify(msg.data)}`);
            //deviceAddr is also available with options.ID or options.friendly_name
            let newK = 1;
            if (options.hasOwnProperty('k_factor'))
                newK = options.k_factor;
            else
                newK =msg.data['presentValue'];
        
            const endpointWaterMeter = msg.device.getEndpoint(WATERMETER_EP);
            //meta.logger.info(`Reading Multiplier k_factor=${JSON.stringify(options.k_factor)}`);
            settings.changeEntityOptions(endpointWaterMeter.deviceIeeeAddress, { k_factor: newK });

        }

        return result;
    },
}

// Events ///////////////////////////////////////////////////////////////////////////

async function onEventCallback(event) {

    // Catch deviceOptionsChanged event, triggered by user changing device specific settings on GUI
    if(event.type == 'deviceOptionsChanged'){
        
        //logger.info(`+-+-+-+-+-+-+-+-+-+-+-+- deviceOptionsChanged event.data =${JSON.stringify(event.data)}`)
        
        // Warning this event is trigger twice, the 2nd with from field = to field
        // Return if there is no from or no to field
        if ((!event.data.hasOwnProperty('from'))||(!event.data.hasOwnProperty('to')))
            return; 

        if (event.data.to.hasOwnProperty('k_factor'))
        {
            // Event seems to be trigger twice, the second one with same value 'from' and 'to'
            if (event.data.from['k_factor'] != event.data.to['k_factor'])
            {
                const newK = event.data.to['k_factor'];

                if(event.data.state.hasOwnProperty('options'))
                    event.data.state.options['k_factor'] = newK;
                else
                    event.data.state.options = {k_factor: newK};

                // Sent the new value to the device so it will be saved on nvm
                const endpointWaterMeter = event.data.device.getEndpoint(WATERMETER_EP);
                await endpointWaterMeter.write('genAnalogValue',  {presentValue: newK});
            } // from != to

        } 
        
        if (event.data.to.hasOwnProperty('upstream_pressure_calibration')) {
            // Event seems to be trigger twice, the second one with same value 'from' and 'to'
            if (event.data.from['upstream_pressure_calibration'] != event.data.to['upstream_pressure_calibration'])
            {
                const newCalibration = event.data.to['upstream_pressure_calibration'];
                logger.info(`+-+-+-+-+-+-+-+-+-+-+-+- UPSTREAM config change msg data =${newCalibration}`)

                if(event.data.state.hasOwnProperty('options'))
                    event.data.state.options['upstream_pressure_calibration'] = newCalibration;
                else
                    event.data.state.options = {upstream_pressure_calibration: newCalibration};

                // Sent the new value to the device so it will be saved on nvm
                const endpointUpstream = event.data.device.getEndpoint(UPSTREAM_EP);
                await endpointUpstream.write('genAnalogValue',  {presentValue: newCalibration});
            } // from != to

        } // has k_factor

    }  //deviceOptionsChanged
}

// Definition ///////////////////////////////////////////////////////////////////////////

const definition = {
    zigbeeModel: ['WaterTankMonitor'],
    model: 'WaterTankMonitor',
    vendor: 'AkiraCorp',
    description: 'Water Tank Monitor Device https://github.com/akira215/zTank',
    fromZigbee: [fz.on_off, fromZigbee_Metering, fromZigbee_kFactor, fromZigbee_Pressure],
    toZigbee: [tz.on_off, toZigbee_Metering, toZigbee_Pressure],
    exposes: [ e.switch(), ...genZtank(), e.temperature()],
    options:[genWaterMeterOptions(), genUpstreamPressureOptions(), genDownstreamPressureOptions()],
    configure: async (device, coordinatorEndpoint, logger) => {
        
        // Get endpoints
        const endpointWaterMeter    = device.getEndpoint(WATERMETER_EP);
        const endpointUpstream      = device.getEndpoint(UPSTREAM_EP);
        const endpointDownstream    = device.getEndpoint(DOWNSTREAM_EP);
        
        // Bind cluster
        await endpointWaterMeter.bind(['msFlowMeasurement'], coordinatorEndpoint);
        await endpointWaterMeter.bind(['genAnalogValue'], coordinatorEndpoint);

        await endpointUpstream.bind(['msPressureMeasurement'], coordinatorEndpoint);
        await endpointUpstream.bind(['genAnalogValue'], coordinatorEndpoint);

        await endpointDownstream.bind(['msPressureMeasurement'], coordinatorEndpoint);
        await endpointDownstream.bind(['genAnalogValue'], coordinatorEndpoint);

        // trigger a read k_factor at startup to update the k_factor from the saved state
        await endpointWaterMeter.read('genAnalogValue', ['presentValue']);
        await endpointUpstream.read('genAnalogValue', ['presentValue']);
        await endpointDownstream.read('genAnalogValue', ['presentValue']);

        // read value on start up
        await endpointWaterMeter.read('msFlowMeasurement', ['measuredValue']);
        await endpointUpstream.read('msPressureMeasurement', ['measuredValue']);
        await endpointDownstream.read('Pressure', ['measuredValue']);

        device.powerSource = 'Mains (single phase)';
        device.save();
    },
    onEvent: onEventCallback
    //ota: ota.zigbeeOTA
};

module.exports = definition;