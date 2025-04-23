// controllers/userController.js
const User = require('../models/user');
const passport = require('passport');
const { body, validationResult } = require('express-validator');
const bcrypt = require('bcryptjs');


const sign_up_get=(req, res)=>{
  res.render('sign-up', {title:'Sign Up', errors:null, user:null});
}

// sign-up form handle
const sign_up_post = [
  body('first_name').trim().isLength({min:1}).escape().withMessage('First name must be given'),
  body('last_name').trim().isLength({ min: 1 }).escape().withMessage('Last name must be specified.'),
  body('email').trim().isLength({ min: 1 }).escape().withMessage('Email must be specified.').isEmail().withMessage('Email must be valid.'),
  body('password').isLength({ min: 6 }).withMessage('Password must be at least 6 characters.'),
  body('confirm_password').custom((value, { req }) => {
    if (value !== req.body.password) {
      throw new Error('Passwords do not match');
    }
    return true;
  }),

  async (req, res, next) => {
    try {
      const errors = validationResult(req);
      const user = {
        first_name: req.body.first_name,
        last_name: req.body.last_name,
        email: req.body.email,
        password: req.body.password,
      };

      if (!errors.isEmpty()) {
        return res.render('sign-up', {
          title: 'Sign Up',
          user,
          errors: errors.array(),
          //send user errors
        });
      }

      const existingUser = await User.findOne({ where: { email: req.body.email } });
      if (existingUser) {
        return res.render('sign-up', {
          title: 'Sign Up',
          user,
          errors: [{ msg: 'Email already in use' }],
        });
      }

      await User.create(user);
      
      res.redirect('/users/log-in');
    } catch (err) {
      return next(err);
    }
  },
];

// Display log-in form
const log_in_get = (req, res) => {
  res.render('log-in', { title: 'Log In', errors: req.flash('error') });
};

// Handle log-in form submission
const log_in_post = passport.authenticate('local', {
  successRedirect: '/',
  failureRedirect: '/users/log-in',
  failureFlash: true,
});



const log_out= async (req, res, next)=>{
  req.logout((err)=>{
    if(err) return next(err);
    res.redirect('/');
  });
};
// Display join club form
const join_club_get = async (req, res) => {
  if (!req.isAuthenticated()) {
    return res.redirect('/users/log-in');
  }
  
  res.render('join-club', { title: 'Join the Club', errors: null });
};

// join club form submission
const join_club_post = [
  body('secret_code').trim().isLength({ min: 1 }).escape().withMessage('Secret code must be specified.'),
  
  async (req, res, next) => {
    try {
      const errors = validationResult(req);
      
      if (!errors.isEmpty()) {
        return res.render('join-club', {
          title: 'Join the Club',
          errors: errors.array(),
        });
      }
      
      const secretCode = req.body.secret_code;
      
      if (secretCode !== 'clun-join-code') {
        return res.render('join-club', {
          title: 'Join the Club',
          errors: [{ msg: 'Incorrect secret code' }],
        });
      }

      //update user status
      const user = await User.findByPk(req.user.id, {$set:{membership_status:'member'},})
      await user.save();
      
      res.redirect('/');
    } catch (err) {
      res.render('/');
    }
  },
];

// display admin form
const admin_get = (req, res) => {
  if (!req.isAuthenticated()) {
    return res.redirect('/users/log-in');
  }
  
  res.render('admin', { title: 'Become an Admin', errors: null });
};

const admin_post = [
  body('admin_code').trim().isLength({ min: 1 }).escape().withMessage('Admin code must be specified.'),
  
  async (req, res, next) => {
    try {
      const errors = validationResult(req);
      
      if (!errors.isEmpty()) {
        return res.render('admin', {
          title: 'Become an Admin',
          errors: errors.array(),
        });
      }
      
      const adminCode = req.body.admin_code;
      
      if (adminCode !== 'supersecretadmin') {
        return res.render('admin', {
          title: 'Become an Admin',
          errors: [{ msg: 'Incorrect admin code' }],
        });
      }
      
      const user = await User.findByPk(req.user.id);
      user.membership_status = 'admin';
      await user.save();
      
      res.redirect('/');
    } catch (err) {
      return next(err);
    }
  },
];

module.exports = {
  sign_up_get,
  sign_up_post,
  log_in_get,
  log_in_post,
  log_out,
  join_club_get,
  join_club_post,
  admin_get,
  admin_post,
};